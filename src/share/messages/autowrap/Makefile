.PHONY: setup_test test clean

default: test

setup_test:
	$(MAKE) -C test

test: setup_test
	python -m unittest discover test *_test.py

clean:
	rm *.pyc
	$(MAKE) clean -C test
