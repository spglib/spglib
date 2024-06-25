# How to write spglib documentation

This directory contains python-sphinx documentation source.

## How to compile

Python>=3.9 is required.

```shell
$ pip install -e ".[docs]"
$ sphinx-autobuild docs docs/_build
```

## Source files

- `conf.py` contains the sphinx setting configuration.
- `*.rst` are the usual sphinx documentation source and the filenames without `.rst` are the keys to link from toctree mainly in `index.rst`.
- `*.inc` are the files included in the other `*.rst` files.

### How to add references

- `references.bib` contains citation entries for [sphinxcontrib-bibtex](https://sphinxcontrib-bibtex.readthedocs.io/en/latest/index.html)
- Each subsection in `references.md` has a bibliography as follows
  - use `cite:empty` directive to refer to an entry in `references.bib`
  - set `keyprefix` for each subsection and append it when refer to bibtex's entries.

````
    {cite:empty}`gen-ITA2016`

    ```{bibliography}
    :cited:
    :keyprefix: gen-
    ```
````

## How to publish

Web page files are copied to `gh-pages` branch. At the spglib github top directory,

```
git checkout gh-pages
rm -r .buildinfo .doctrees *
```

From the directory the sphinx doc is complied, html sources are stored
in `_build/html`. These files are copied to gh-pages by

```
rsync -avh _build/html/ <spglib-repository-directory>/
```

Again, at the spglib github top directory,

```
git add .
git commit -a -m "Update documentation ..."
git push
```
