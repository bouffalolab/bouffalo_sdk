project = "AT Module"
author = "Bouffalo Lab"
copyright = "2026, Bouffalo Lab"
root_doc = "index"
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
templates_path = ["_templates"]
html_logo = "_static/bouffalolab-logo.png"
html_theme_options = {"logo_only": True}
html_css_files = ["brand.css"]
latex_engine = "xelatex"
latex_documents = [("index", "atmodule.tex", "AT Command Set", author, "manual")]
latex_elements = {
    "preamble": r"""
\usepackage{xeCJK}
\setCJKmainfont{Noto Serif CJK SC}
\setCJKsansfont{Noto Sans CJK SC}
\setCJKmonofont{Noto Sans Mono CJK SC}
""",
}
