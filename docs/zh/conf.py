# Configuration file for the Sphinx documentation builder.

import os
import os.path
import subprocess
import shutil

os.environ[r'BL_SDK_BASE'] = os.path.abspath(r'../..')
os.environ[r'DOXYGEN_OUTPUT_DIR'] = os.path.abspath(r'_static')

def doxygen_run(bl_sdk_path):
    shutil.rmtree(os.environ[r'DOXYGEN_OUTPUT_DIR'], ignore_errors=True)
    doxygen_conf = os.path.join(bl_sdk_path, r'docs/doxygen/Doxyfile')
    doxygen_cmd = r'doxygen ' + doxygen_conf
    print(os.path.abspath(doxygen_conf))
    return_code, out = subprocess.getstatusoutput(doxygen_cmd)
    #print(out)
    
    if 0 != return_code:
        print("Doxygen failed!!!")
        print(out)


doxygen_run(os.environ[r'BL_SDK_BASE'])

# -- Project information

project = 'BouffaloSDK'
copyright = '2022, Bouffalolab Team'
author = 'Bouffalolab Team'

release = '2.0'
version = '2.0.0'

# -- General configuration

extensions = [
    'sphinx.ext.duration',
    'sphinx.ext.doctest',
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
    'sphinx.ext.intersphinx',
    'recommonmark',
    'sphinx_markdown_tables'
]

intersphinx_mapping = {
#    'python': ('https://docs.python.org/3/', None),
#    'sphinx': ('https://www.sphinx-doc.org/en/master/', None),
}
intersphinx_disabled_domains = ['std']

templates_path = ['_templates']

# -- Options for HTML output

html_theme = 'sphinx_rtd_theme'
