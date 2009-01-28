
R=R
R_FILES = $(wildcard *.R)
R_OPTS = --no-save
CAT = cat

graphs :
	for rf in $(R_FILES); do $(R) $(R_OPTS) < $$rf; done;

clean:
	$(RM) *.pdf