from spglib import __version__ as version

project = "Spglib"
copyright = "2009, Atsushi Togo"

extensions = [
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode",
    "sphinxcontrib.bibtex",
    "myst_parser",
    "autodoc2",
    "sphinx.ext.doctest",
    "sphinx.ext.extlinks",
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
html_title = f"Spglib v{version}"
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
# Autodoc2
# -----------------------------------------------------------------------------
autodoc2_output_dir = "api/python-api"
autodoc2_render_plugin = "myst"
autodoc2_docstring_parser_regexes = [
    (r".*", "rst"),
]
autodoc2_annotations = False
autodoc2_packages = [
    {
        "path": "../python/spglib",
        "module": "spglib",
    },
]
autodoc2_hidden_objects = ["dunder", "private", "inherited"]
autodoc2_hidden_regexes = [
    "spglib.spglib.get_pointgroup",
    # Layer group
    "spglib.spglib.get_layergroup",
    "spglib.spglib.get_symmetry_layerdataset",
    # Kpoints
    "spglib.spglib.get_grid_point_from_address",
    "spglib.spglib.get_stabilized_reciprocal_mesh",
    "spglib.spglib.get_grid_points_by_rotations",
    "spglib.spglib.get_BZ_grid_points_by_rotations",
    "spglib.spglib.relocate_BZ_grid_address",
]

# -----------------------------------------------------------------------------
# linkcheck
# -----------------------------------------------------------------------------

linkcheck_ignore = [
    # Ignore closed-access links
    r"https://www.sciencedirect.com/.*",
    r"https://www.jstor.org/.*",
    # Cannot check for 403 error only
    r"https://doi.org/10.1002/qua.20747",
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
