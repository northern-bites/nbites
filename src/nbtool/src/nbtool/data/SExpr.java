package nbtool.data;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import nbtool.util.U;

/*
 * The general contract is that read operations such as get and find will succeed, but the result may not exist.
 * 
 * Modifications to SExprs that are not present in the expression throws DoesNotExist, which is an 
 * uncaught exception.
 * 
 * value* operations will also throw DNE.
 * */

public abstract class SExpr implements Serializable{
	
	public static final char[] SPECIAL = {' ', '(', ')', '\r', '\n', '\t'};
	
	private static int indexOfFirst(char[] opts, String target) {
		return indexOfFirstFrom(opts, target, 0);
	}
	
	private static int indexOfFirstFrom(char[] opts, String target, int from) {
		String sopts = new String(opts);
		for (int i = from; i < target.length(); ++i) {
			if (sopts.contains(target.substring(i, i + 1)))
				return i;
		}
		
		return -1;
	}
	
	private static final SExpr NOT_FOUND = new NotFound();
	
	public static SExpr deserializeFrom(String serial) {
		return _deserialize(serial, new MutRef());
	}
	
	private static SExpr _deserialize(String s, MutRef p) {
		// Skip whitespace
	      while (p.val < s.length() && Character.isWhitespace(s.charAt(p.val)))
	        ++p.val;
	      
	      if (p.val >= s.length())
	        return NOT_FOUND;

	      int q;
	      SExpr se;

	      switch (s.charAt(p.val))
	      {
	        case '"':
	          // Atoms starting with "
	          String name = "";
	          while (p.val < s.length() && s.charAt(p.val) == '"')
	          {
	            if (name.length() > 0)
	              name += '"';
	            q = s.indexOf('"', ++p.val);
	            if (q < 0)
	              q = s.length();
	            name += s.substring(p.val, q);
	            if (q < s.length())
	              ++q;
	            p.val = q;
	          }
	          
	          return SExpr.newAtom(name);

	        case '(':
	          // Lists
	          ++p.val;
	          se = SExpr.newList();
	          while (true)
	          {
	            SExpr e = _deserialize(s, p);
	            if (e == null)
	              break;
	            se.append(e);
	          }
	          
	          if (p.val < s.length() && s.charAt(p.val) == ')')
	            ++p.val;
	          return se;

	        case ')':
	          // List termination
	          return null;

	        default:
	          // Atoms not starting with ""
	          q = indexOfFirstFrom(SPECIAL, s, p.val);
	          if (q < 0)
	            q = s.length();
	          se = SExpr.newAtom(s.substring(p.val, q));
	          p.val = q;
	          return se;
	      }
	}
	
	
	public static SExpr newAtom(String val) {
		return new Found(val);
	}
	
	public static SExpr newAtom(Object v) {
		return SExpr.newAtom(v.toString());
	}
	
	public static SExpr newList() {
		return new Found(Arrays.asList(new SExpr[]{}));
	}
	
	public static SExpr newList(SExpr ... contents) {
		return new Found(Arrays.asList(contents));
	}
	
	public static SExpr newKeyValue(String key, String value) {
		return newList(new Found(key), new Found(value));
	}
	
	public abstract boolean isAtom();
	public abstract boolean exists();
	
	public abstract int count();
	public abstract SExpr get(int i);
	public abstract void append(SExpr ... exprs);
	
	public abstract String value();
	public abstract int valueAsInt() throws NumberFormatException;
	public abstract long valueAsLong() throws NumberFormatException;
	public abstract double valueAsDouble() throws NumberFormatException;
	public abstract boolean valueAsBoolean();
	
	public abstract String print();
	public abstract String print(int level);
	public abstract String serialize();
	
	public abstract SExpr find(String key);
	
	private static class Found extends SExpr {
		
		protected Found(List<SExpr> l) {
			atom = false;
			list = new ArrayList<SExpr>(l);
		}
		
		protected Found(String v) {
			atom = true;
			value = v;
		}
		
		private boolean atom;
		private String value;
		private ArrayList<SExpr> list;

		@Override
		public boolean isAtom() {
			return atom;
		}

		@Override
		public boolean exists() {
			return true;
		}

		@Override
		public int count() {
			return atom ? -1 : list.size();
		}

		@Override
		public SExpr get(int i) {
			if (!atom && i < list.size())
				return list.get(i);
			else return NOT_FOUND;
		}

		@Override
		public void append(SExpr... exprs) {
			if (!atom) {
				for (SExpr sexpr : exprs)
					list.add(sexpr);
			}
		}

		@Override
		public String value() {
			return atom ? value : null;
		}

		@Override
		public int valueAsInt() throws NumberFormatException {
			return Integer.parseInt(value);
		}

		@Override
		public long valueAsLong() throws NumberFormatException {
			return Long.parseLong(value);
		}

		@Override
		public double valueAsDouble() throws NumberFormatException {
			return Double.parseDouble(value);
		}
		
		@Override
		public boolean valueAsBoolean() {
			if (value.trim().equalsIgnoreCase("true"))
				return true;
			if (value.trim().equalsIgnoreCase("false"))
				return false;
			if (value.trim().equalsIgnoreCase("1"))
				return true;
			if (value.trim().equalsIgnoreCase("0"))
				return false;
			throw new NumberFormatException();
		}

		private final int indent = 2;
		private final int linelimit = 64;
		@Override
		public String print() {
			return print(0);
		}
		
		@Override
		public String print(int level) {
			String prefix = new String(new char[level * indent]).replace("\0", " ");
			
			String s = serialize();
			if (atom || s.length() + (indent * level) <= linelimit)
				return prefix + s;

			s = prefix + "(";
			for (int i = 0; i < count(); ++i) {
				String e = list.get(i).print(level + 1);

				if (i > 0)
				{
					s += '\n';
					s += e;
				}
				else
					s += e.substring(indent * level + 1);
			}

			s += "\n";
			s += prefix;
			s += ")";
			return s;
		}

		@Override
		public String serialize() {
			
			if (atom) {
				if (indexOfFirst(SPECIAL, value) < 0)
					return value;
				return "\"" + value.replaceAll("\"", "\"\"") + "\"";
			}
			
			String s = "(";
			for (SExpr sex : list) {
				if (s.length() > 1)
					s += ' ';
				s += sex.serialize();
			}
			
			s += ')';
			return s;
		}

		@Override
		public SExpr find(String key) {

			if (atom)
				return NOT_FOUND;

			for (SExpr s : list) {
				if (	s.count() > 0 &&
						s.get(0).isAtom() &&
						s.get(0).value().equals(key)
						)
					return s;
			}

			return NOT_FOUND;
		}
	}
	
	private static class NotFound extends SExpr {

		@Override
		public boolean isAtom() {
			return true;	//Atom implies less.
		}

		@Override
		public boolean exists() {
			return false;
		}

		@Override
		public int count() {
			return -1;
		}

		@Override
		public SExpr get(int i) {
			return this;
		}

		@Override
		public void append(SExpr... exprs) {
			throw new DoesNotExistException();
		}

		@Override
		public String value() {
			throw new DoesNotExistException();
		}

		@Override
		public int valueAsInt() throws NumberFormatException {
			throw new DoesNotExistException();
		}

		@Override
		public long valueAsLong() throws NumberFormatException {
			throw new DoesNotExistException();
		}

		@Override
		public double valueAsDouble() throws NumberFormatException {
			throw new DoesNotExistException();
		}
		
		@Override
		public boolean valueAsBoolean() {
			throw new DoesNotExistException();
		}

		@Override
		public String print() {
			throw new DoesNotExistException();
		}
		
		@Override
		public String print(int level) {
			throw new DoesNotExistException();
		}

		@Override
		public String serialize() {
			throw new DoesNotExistException();
		}

		@Override
		public SExpr find(String key) {
			return this;
		}
	}
	
	public static class DoesNotExistException extends RuntimeException {}
	
	private static class MutRef {
		int val;
		MutRef() {
			val = 0;
		}
	}
	
	
	public static void main(String[] args) {
		String ser = "(the list goes (on and on and \"what?()\" ()))";
		
		SExpr s = SExpr.deserializeFrom(ser);
		
		U.w("done...");
		
		U.wf("%s\n",s.print());
	}
}
