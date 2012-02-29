#!/usr/bin/env python

try:
    from setuptools import setup
except ImportError:
    # In case the system doesn't have setuptools
    import distribute_setup
    distribute_setup.use_setuptools()

    from setuptools import setup


__version__ = 0.1

setup(name='unity-launcher-editor',
        version=__version__,
        packages=['unitylaunchereditor'],
        install_requires=[
            'setuptools',
            'pygtk',
            'gtk',
            'pkg_resources',
        ],
        include_package_data=True,
        entry_points=dict(
            gui_scripts=[
                'unity-launcher-editor = unitylaunchereditor.main:main',
            ]),
)
