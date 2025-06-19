import importlib.metadata

project = "Spglib"
copyright = "2009, Atsushi Togo"

extensions = [
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode",
    "sphinx.ext.autodoc",
    "sphinx_autodoc_typehints",
    "sphinxcontrib.bibtex",
    "myst_parser",
    "sphinx.ext.doctest",
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinx_tippy",
]

exclude_patterns = [
    "README.md",
    "_build",
]
source_suffix = {
    ".md": "markdown",
    ".rst": "restructuredtext",
}

# -----------------------------------------------------------------------------
# MyST
# -----------------------------------------------------------------------------

myst_enable_extensions = [
    "amsmath",
    "dollarmath",
    "html_admonition",
    "html_image",
    "replacements",
    "smartquotes",
    "tasklist",
    "colon_fence",
]

myst_dmath_double_inline = True
myst_heading_anchors = 3

# -----------------------------------------------------------------------------
# BibTeX
# -----------------------------------------------------------------------------

bibtex_bibfiles = ["references.bib"]
bibtex_default_style = "unsrt"

# -----------------------------------------------------------------------------
# Sphinx-book-theme
# -----------------------------------------------------------------------------

html_theme = "sphinx_book_theme"
html_title = f"Spglib v{importlib.metadata.version('spglib')}"
html_theme_options = {
    # https://sphinx-book-theme.readthedocs.io/en/latest/reference.html
    "repository_url": "https://github.com/spglib/spglib",
    "use_repository_button": True,
    "navigation_with_keys": True,
    "globaltoc_includehidden": "true",
    "show_toc_level": 3,
}
html_static_path = ["_static"]

# -----------------------------------------------------------------------------
# linkcheck
# -----------------------------------------------------------------------------

linkcheck_ignore = [
    # Ignore closed-access links
    r"https://www.sciencedirect.com/.*",
    r"https://www.jstor.org/.*",
    # Cannot check for 403 error only
    r"https://doi.org/10.1002/qua.20747",
    r"https://doi.org/10.1080/27660400.2024.2384822",
    # Getting 403 error (maybe because cloudflare)
    r"https://.*\.iucr.org/",
    # No need to check these
    r"https://github.com/spglib/spglib/pull/.*",
]
linkcheck_allowed_redirects = {
    r"https://doi.org/.*": r".*",
    r"https://github.com/spglib/spglib/archive/.*": r".*",
    r"https://spglib.readthedocs.io/.*": r"https://spglib.readthedocs.io/.*",
    r"https://docs.rs/crate/spglib/": r"https://docs.rs/crate/spglib/.*",
    r"https://github.com/chrisjsewell/sphinx-autodoc2": r"https://github.com/sphinx-extensions2/sphinx-autodoc2",
    r"https://github.com/spglib/spglib/tree/.*": r"https://github.com/spglib/spglib/blob/.*",
}
linkcheck_anchors_ignore_for_url = [
    r"https://github.com",
]

# -----------------------------------------------------------------------------
# extlinks
# -----------------------------------------------------------------------------

extlinks = {
    "issue": ("https://github.com/spglib/spglib/issues/%s", "issue %s"),
    "path": ("https://github.com/spglib/spglib/tree/develop/%s", "%s"),
    "user": ("https://github.com/%s", "%s"),
}

# -----------------------------------------------------------------------------
# intersphinx and tippy
# -----------------------------------------------------------------------------

intersphinx_mapping = {
    "cmake": ("https://cmake.org/cmake/help/latest", None),
    "scikit": ("https://scikit-build-core.readthedocs.io/en/latest/", None),
}

tippy_rtd_urls = [
    # Only works with RTD hosted intersphinx
    # "https://cmake.org/cmake/help/latest",
    "https://scikit-build-core.readthedocs.io/en/latest/",
]
