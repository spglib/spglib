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
    "autodoc2",
]

exclude_patterns = [
    "README.md",
    "_build",
]
source_suffix = {
    ".md": "markdown",
}

# -----------------------------------------------------------------------------
# MyST
# -----------------------------------------------------------------------------

myst_enable_extensions = [
    "amsmath",
    "dollarmath",
    "html_admonition",
    "html_image",
    "linkify",
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
html_title = f"Spglib v{release}"
html_theme_options = {
    # https://sphinx-book-theme.readthedocs.io/en/latest/reference.html
    "navigation_with_keys": True,
    "globaltoc_includehidden": "true",
    "show_toc_level": 3,
}
html_static_path = ["_static"]

# -----------------------------------------------------------------------------
# Autodoc2
# -----------------------------------------------------------------------------
autodoc2_output_dir = "python-interface/python-apidocs"
autodoc2_render_plugin = "myst"
autodoc2_packages = [
    {
        "path": "../python/spglib/spglib.py",
        "module": "spglib",
    }
]
autodoc2_docstring_parser_regexes = [
    (r".*", 'rst'),
]
autodoc2_hidden_objects = ['undoc', 'dunder', 'private', 'inherited']
autodoc2_hidden_regexes = [
    "spglib.get_pointgroup",
    # Layer group
    "spglib.get_layergroup",
    "spglib.get_symmetry_layerdataset",
    # Kpoints
    "spglib.get_grid_point_from_address",
    "spglib.get_stabilized_reciprocal_mesh",
    "spglib.get_grid_points_by_rotations",
    "spglib.get_BZ_grid_points_by_rotations",
    "spglib.relocate_BZ_grid_address",
]
