.PHONY: setup_test test

default: test

setup_test:
	$(MAKE) -C test

test: setup_test
	python -m test.parser_test
	python -m test.wrapped_code_test
