from flask import Flask, request, jsonify, make_response, send_file, abort
import ssl
import argparse
import os

app = Flask(__name__)

# Set the allowed root directory (current directory)
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
ALLOWED_DIR = BASE_DIR  # Allow access to current directory and its subdirectories

@app.route('/', methods=['GET', 'POST', 'PUT', 'DELETE', 'PATCH'])
def root():
    """Handle root path requests"""
    return echo()

@app.route('/<path:subpath>', methods=['GET', 'POST', 'PUT', 'DELETE', 'PATCH'])
def handle_path(subpath):
    """Handle all path requests, including file access"""
    # If it's a GET request, try to handle as file request
    if request.method == 'GET':
        # Construct absolute path
        abs_path = os.path.abspath(os.path.join(ALLOWED_DIR, subpath))

        # Security check: ensure requested path is within allowed directory
        if not abs_path.startswith(ALLOWED_DIR):
            return jsonify({'error': 'Access denied'}), 403

        # If it's a file and exists, return the file
        if os.path.isfile(abs_path):
            try:
                # Get file size
                file_size = os.path.getsize(abs_path)
                # Send file and add file size header
                response = send_file(abs_path, as_attachment=False)
                response.headers['X-File-Size'] = str(file_size)
                response.headers['Content-Length'] = str(file_size)
                return response
            except Exception as e:
                return jsonify({'error': str(e)}), 500

    # If not a file request or not GET method, return echo functionality
    return echo(subpath)

def echo(subpath=None):
    """Endpoint to handle all requests"""
    # Collect request information
    request_data = {
        'method': request.method,
        'path': request.path,
        'headers': dict(request.headers),
        'args': request.args.to_dict(),
        'form': request.form.to_dict(),
        'data': request.data.decode('utf-8') if request.data else None,
        'json': request.json if request.is_json else None,
        'cookies': request.cookies.to_dict(),
        'remote_addr': request.remote_addr,
        'subpath': subpath,
    }

    # Print request info (for debugging)
    print("\n=== Received Request ===")
    print(f"Method: {request.method}")
    print(f"Path: {request.path}")
    print(f"Subpath: {subpath}")
    print("\nHeaders:")
    for key, value in request.headers.items():
        print(f"  {key}: {value}")

    if request.args:
        print("\nQuery Parameters:")
        for key, value in request.args.items():
            print(f"  {key}: {value}")

    if request.form:
        print("\nForm Data:")
        for key, value in request.form.items():
            print(f"  {key}: {value}")

    if request.data:
        print("\nRaw Data:")
        print(f"  {request.data.decode('utf-8')}")

    if request.is_json:
        print("\nJSON Data:")
        print(f"  {request.json}")

    # Construct response
    response = make_response(jsonify(request_data))
    response.headers['X-Test-Server'] = 'Flask-Echo-Server'
    response.set_cookie('test_cookie', 'cookie_value')
    return response

def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='HTTP/HTTPS Test Server')
    parser.add_argument('--host', default='0.0.0.0', help='Host address to listen on')
    parser.add_argument('--port', type=int, default=5000, help='Port to listen on')
    parser.add_argument('--https', action='store_true', help='Enable HTTPS')
    parser.add_argument('--cert', help='SSL certificate file')
    parser.add_argument('--key', help='SSL private key file')
    return parser.parse_args()

def generate_self_signed_cert():
    """Generate self-signed certificate"""
    import subprocess
    if not os.path.exists('cert.pem') or not os.path.exists('key.pem'):
        print("Generating self-signed certificate...")
        subprocess.run([
            'openssl', 'req', '-x509', '-newkey', 'rsa:4096',
            '-nodes', '-out', 'cert.pem', '-keyout', 'key.pem',
            '-days', '365', '-subj', '/CN=localhost'
        ], check=True)
    return 'cert.pem', 'key.pem'

if __name__ == '__main__':
    args = parse_args()

    if args.https:
        cert_file, key_file = args.cert, args.key
        if not cert_file or not key_file:
            cert_file, key_file = generate_self_signed_cert()
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ssl_context.load_cert_chain(cert_file, key_file)
        ssl_context.set_ciphers('ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384')
    else:
        ssl_context = None

    print(f"Starting {'HTTPS' if args.https else 'HTTP'} server...")
    print(f"Access URL: {'https' if args.https else 'http'}://{args.host}:{args.port}")
    print("Features:")
    print("1. Access paths like /path/to/file.txt to get file content")
    print("2. Other HTTP methods (POST/PUT etc) or non-file paths will return request info")
    print("3. Supports all HTTP methods and arbitrary paths")

    app.run(
        host=args.host,
        port=args.port,
        ssl_context=ssl_context,
        debug=True
    )
