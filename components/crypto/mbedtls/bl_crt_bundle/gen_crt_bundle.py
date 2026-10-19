#!/usr/bin/env python3
"""
Certificate Bundle Generator for BL SDK

Generates custom certificate bundle format from PEM/DER certificates.
Bundle format: [count][entry1][entry2]...
  count: 2 bytes (big-endian)
  entry: [name_len(2)][key_len(2)][subject_name][pub_key]
"""

import argparse
import csv
import os
import re
import struct
import sys

try:
    from cryptography import x509
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives import serialization
except ImportError:
    sys.stderr.write('Error: cryptography package required\n')
    sys.stderr.write('Install: pip install cryptography\n')
    sys.exit(1)


class BundleConfig:
    """Configuration for bundle generation"""
    DEFAULT_OUTPUT = 'x509_crt_bundle'
    MAX_CERTS = 500


class MsgWriter:
    """Message output handler"""
    def __init__(self, silent=False):
        self.silent = silent

    def info(self, msg):
        if not self.silent:
            self._write(msg, stream=sys.stderr)

    def error(self, msg):
        self._write(msg, stream=sys.stderr)

    @staticmethod
    def _write(msg, stream):
        stream.write(f'[cert_bundle] {msg}\n')


class CertEntry:
    """Single certificate entry representation"""
    def __init__(self, cert_obj):
        self.cert = cert_obj
        self._extract_data()

    def _extract_data(self):
        pub_key = self.cert.public_key()
        self.pubkey_der = pub_key.public_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
        self.subject_der = self.cert.subject.public_bytes(default_backend())

    @property
    def sort_key(self):
        return self.subject_der

    def serialize(self):
        """Convert to binary bundle entry format"""
        header = struct.pack('>HH', len(self.subject_der), len(self.pubkey_der))
        return header + self.subject_der + self.pubkey_der


class PemParser:
    """PEM format certificate parser"""

    BEGIN_MARKER = '-----BEGIN CERTIFICATE-----'
    END_MARKER = '-----END CERTIFICATE-----'

    @classmethod
    def split_certs(cls, content):
        """
        Split PEM content into individual certificates
        Returns list of PEM strings
        """
        certs = []
        current = []
        in_cert = False

        for line in content.splitlines():
            if line.strip() == cls.BEGIN_MARKER:
                current = [line]
                in_cert = True
            elif line.strip() == cls.END_MARKER:
                current.append(line)
                in_cert = False
                if current:
                    certs.append('\n'.join(current))
            elif in_cert:
                current.append(line)

        return certs

    @classmethod
    def parse_with_filter(cls, content, filter_set):
        """
        Parse Mozilla-style cacrt_all.pem with filter
        Format: Name\n====\n...certificate...
        """
        pattern = r'(^.+?)\n(=+\n[\s\S]+?END CERTIFICATE-----\n)'
        matches = re.findall(pattern, content, re.MULTILINE)

        filtered = []
        for name, cert_pem in matches:
            if name in filter_set:
                filtered.append(cert_pem)

        return filtered


class CertCollector:
    """Collects certificates from various sources"""

    def __init__(self, msg_writer):
        self.msg = msg_writer
        self.cert_list = []

    def load_file(self, filepath):
        """Load certificates from a single file"""
        ext = os.path.splitext(filepath)[1].lower()

        if ext == '.pem':
            return self._load_pem(filepath)
        elif ext == '.der':
            return self._load_der(filepath)
        return False

    def _load_pem(self, filepath):
        self.msg.info(f'Processing PEM: {filepath}')
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        cert_blocks = PemParser.split_certs(content)
        if not cert_blocks:
            raise ValueError(f'No certificates found in {filepath}')

        loaded = 0
        for block in cert_blocks:
            try:
                cert_obj = x509.load_pem_x509_certificate(
                    block.encode('utf-8'),
                    default_backend()
                )
                self.cert_list.append(cert_obj)
                loaded += 1
            except Exception as e:
                self.msg.error(f'Skip invalid cert in {filepath}: {e}')

        self.msg.info(f'Loaded {loaded} cert(s) from {filepath}')
        return loaded > 0

    def _load_der(self, filepath):
        self.msg.info(f'Processing DER: {filepath}')
        with open(filepath, 'rb') as f:
            data = f.read()

        cert_obj = x509.load_der_x509_certificate(data, default_backend())
        self.cert_list.append(cert_obj)
        self.msg.info('Loaded 1 cert from DER file')
        return True

    def load_directory(self, dirpath):
        """Load all certificates from directory"""
        found = False
        for filename in os.listdir(dirpath):
            filepath = os.path.join(dirpath, filename)
            if os.path.isfile(filepath):
                found |= self.load_file(filepath)
        return found

    def load_filtered(self, pem_path, csv_path):
        """Load with CSV filter (for Mozilla cacrt_all.pem)"""
        self.msg.info(f'Processing filtered: {pem_path}')

        # Load filter set from CSV
        filter_names = self._parse_filter_csv(csv_path)

        # Parse PEM with filter
        with open(pem_path, 'r', encoding='utf-8') as f:
            content = f.read()

        filtered_certs = PemParser.parse_with_filter(content, filter_names)

        if not filtered_certs:
            raise ValueError('No certificates matched filter')

        # Load filtered certificates
        for cert_pem in filtered_certs:
            try:
                cert_obj = x509.load_pem_x509_certificate(
                    cert_pem.encode('utf-8'),
                    default_backend()
                )
                self.cert_list.append(cert_obj)
            except Exception as e:
                self.msg.error(f'Skip invalid cert: {e}')

        self.msg.info(f'Loaded {len(filtered_certs)} filtered cert(s)')

    @staticmethod
    def _parse_filter_csv(csv_path):
        """Parse CSV file, extract certificate names from second column"""
        names = set()
        with open(csv_path, 'r', encoding='utf-8') as f:
            reader = csv.reader(f)
            next(reader)  # Skip header
            for row in reader:
                if len(row) >= 2:
                    names.add(row[1])
        return names


class BundleGenerator:
    """Generates binary certificate bundle"""

    def __init__(self, certificates):
        self.entries = [CertEntry(c) for c in certificates]

    def generate(self):
        """Create binary bundle"""
        # Sort by subject name for binary search
        self.entries.sort(key=lambda e: e.sort_key)

        # Build bundle
        parts = []
        parts.append(struct.pack('>H', len(self.entries)))

        for entry in self.entries:
            parts.append(entry.serialize())

        return b''.join(parts)


def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description='BL SDK Certificate Bundle Generator',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument(
        '-i', '--input',
        nargs='+',
        required=True,
        metavar='PATH',
        help='Certificate files/directories (.pem, .der)'
    )

    parser.add_argument(
        '-f', '--filter',
        metavar='CSV',
        help='CSV filter file for cacrt_all.pem (2nd column: cert names)'
    )

    parser.add_argument(
        '-o', '--output',
        default=BundleConfig.DEFAULT_OUTPUT,
        metavar='FILE',
        help=f'Output bundle file (default: {BundleConfig.DEFAULT_OUTPUT})'
    )

    parser.add_argument(
        '-q', '--quiet',
        action='store_true',
        help='Suppress info messages'
    )

    return parser.parse_args()


def main():
    args = parse_arguments()
    msg = MsgWriter(silent=args.quiet)

    # Collect certificates
    collector = CertCollector(msg)

    for path in args.input:
        if os.path.isfile(path):
            basename = os.path.basename(path)
            if basename == 'cacrt_all.pem' and args.filter:
                collector.load_filtered(path, args.filter)
            else:
                collector.load_file(path)
        elif os.path.isdir(path):
            if not collector.load_directory(path):
                msg.error(f'No valid certificates in directory: {path}')
                sys.exit(1)
        else:
            msg.error(f'Invalid path: {path}')
            sys.exit(1)

    cert_count = len(collector.cert_list)
    if cert_count == 0:
        msg.error('No certificates loaded')
        sys.exit(1)

    if cert_count > BundleConfig.MAX_CERTS:
        msg.error(f'Too many certificates: {cert_count} > {BundleConfig.MAX_CERTS}')
        sys.exit(1)

    msg.info(f'Total certificates: {cert_count}')

    # Generate bundle
    generator = BundleGenerator(collector.cert_list)
    bundle_data = generator.generate()

    # Write output
    with open(args.output, 'wb') as f:
        f.write(bundle_data)

    msg.info(f'Bundle written: {args.output} ({len(bundle_data)} bytes)')


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)
    except Exception as e:
        sys.stderr.write(f'[cert_bundle] Error: {e}\n')
        sys.exit(1)
