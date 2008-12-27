
R=R
R_FILES = $(wildcard *.R)
R_OPTS = "--no-save"
CAT = cat

graphs :
	$(CAT) $(R_FILES) |$(R) $(R_OPTS)

clean:
	$(RM) *.pdf