from importlib.metadata import version

project = "Spglib"
copyright = "2009, Atsushi Togo"
release = version("spglib")  # e.g. "v.1.16.5"
# The short X.Y version.
version = ".".join(release.split(".")[:3])  # e.g. "v.1.16"

extensions = [
    'sphinx.ext.mathjax',
    "sphinx.ext.viewcode",
    "sphinxcontrib.bibtex",
    "myst_parser",
]
myst_enable_extensions = [
    "amsmath",
    "dollarmath",
    "html_admonition",
    "html_image",
    "linkify",
    "replacements",
    "smartquotes",
    "tasklist",
]

exclude_patterns = [
    "README.md",
    "_build",
]
source_suffix = {
    ".md": "markdown",
}

# https://pypi.org/project/sphinxcontrib-bibtex/
bibtex_bibfiles = ["references.bib"]
bibtex_default_style = "unsrt"

myst_dmath_double_inline = True
myst_heading_anchors = 3

html_theme = "sphinx_book_theme"
html_title = f"Spglib v{release}"
html_theme_options = {
    # https://sphinx-book-theme.readthedocs.io/en/latest/reference.html
    "navigation_with_keys": True,
    "globaltoc_includehidden": "true",
}
html_static_path = ["_static"]
