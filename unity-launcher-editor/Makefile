all: clean buildegg

clean:
	find . -type f -name '*.py[co]' -delete
	$(RM) -r build distribute-* dist MANIFEST unity_launcher_editor.egg-info

buildegg:
	python setup.py bdist_egg
