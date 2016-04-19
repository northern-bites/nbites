package nbtool.term.units;

public abstract class UnitParent {
	
	public final boolean testAndPrint(String absPathToResources) {
		System.out.printf("\n= = = = = = Performing unit test '%s.' = = = = = =\n",
				this.toString());
		
		try {
			boolean success = this.test(absPathToResources);
			if (success) {
				System.out.printf("+ + + + + + Test passed.\n");
				return true;
			} else {
				System.out.printf("- - - - - - Test failed.\n");
				return false;
			}
		} catch (UnitFailedException ufe) {
			System.out.printf("- - - - - - Test failed with:\n\t%s\n", ufe.reason);
			ufe.printStackTrace();
			return false;
		} catch (Throwable t) {
			System.out.printf("- - - - - - Test failed with uncaught throwable!!\n\t%s\n", t.getMessage());
			t.printStackTrace();
			return false;
		}
	}
	
	public abstract boolean test(String absPathToResources) throws UnitFailedException;
	
	@Override
	public abstract String toString();
	
	public static class UnitFailedException extends Exception {
		public String reason;
		public UnitFailedException(String reason) {
			this.reason = reason;
		}
	}
	
	protected static void failed(String reason) throws UnitFailedException {
		 throw new UnitFailedException(reason);
	}
	
	protected static void require(boolean condition, String testing) throws UnitFailedException {
		if (!condition) {
			failed(testing);
		}
	}
	
	protected static void requireEqual(Object one, Object two, String testing) throws UnitFailedException {
		if (!one.equals(two)) {
			String full = String.format("failed '%s' {%s} != {%s}", testing, one.toString(), two.toString());
			failed(full);
		}
	}
}
