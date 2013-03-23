.PHONY: setup_test test clean

default: test

setup_test:
	$(MAKE) -C test

test: setup_test
	python -m test.parser_test
	python -m test.wrapped_code_test

clean:
	rm *.pyc
	$(MAKE) clean -C test
