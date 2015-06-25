package nbtool.data;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Vector;

/*
 * The general contract is that read operations such as get and find will succeed, but the result may not exist.
 * 
 * Modifications to SExprs that are not present in the expression throws DoesNotExist, which is an 
 * uncaught exception.
 * 
 * value* operations will also throw DNE.
 * */

public abstract class SExpr implements Serializable{

	//singleton return when operation could not find valid return.
	private static final SExpr NOT_FOUND = new NotFound();

	/**
	 *  FACTORY METHODS */

	public static SExpr deserializeFrom(String serializedSExpr) {
		return _deserialize(serializedSExpr, new MutRef());
	}

	/* in java, fac-methods of form newAtom and atom are equivalent – all objects in java are passed by pointer. */
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

	public static SExpr newKeyValue(String key, SExpr value) {
		return newList(new Found(key), value);
	}

	public static SExpr newKeyValue(String key, int value) {
		return newList(new Found(key), new Found(Integer.toString(value)));
	}

	public static SExpr newKeyValue(String key, float value) {
		return newList(new Found(key), new Found(Float.toString(value)));
	}

	public static SExpr newKeyValue(String key, double value) {
		return newList(new Found(key), new Found(Double.toString(value)));
	}

	public static SExpr atom(String val) {
		return SExpr.newAtom(val);
	}

	public static SExpr atom(Object v) {
		return SExpr.newAtom(v.toString());
	}

	public static SExpr list() {
		return new Found(Arrays.asList(new SExpr[]{}));
	}
	
	public static SExpr stringify(Object ... args) {
		SExpr list = list();
		for (Object o : args) {
			list.append(SExpr.atom(o.toString()));
		}
		
		return list;
	}

	public static SExpr list(SExpr ... contents) {
		return new Found(Arrays.asList(contents));
	}

	public static SExpr pair(String key, String value) {
		return newList(new Found(key), new Found(value));
	}

	public static SExpr pair(String key, SExpr value) {
		return newList(new Found(key), value);
	}

	public static SExpr pair(String key, int value) {
		return newList(new Found(key), new Found(Integer.toString(value)));
	}
	
	public static SExpr pair(String key, double value) {
		return newList(new Found(key), new Found(Double.toString(value)));
	}

	/* checks for recursive trees,  */
	public static SExpr deepCopy(SExpr node) {
		return node.deepCopy();
	}

	/* end factory methods */

	/* instance methods */

	/* modifying TYPE OF SEXPR (type of 'this' after function is as specified by method)*/
	public abstract void setList(List<SExpr> list);
	public abstract void setList(SExpr ... items);
	public abstract void setAtom(String val);

	//list modifications
	public abstract void insert(int index, SExpr item);
	public abstract boolean remove(SExpr item);
	public abstract void append(SExpr ... exprs);

	//list retrieval
	public abstract int count();
	public abstract SExpr get(int i);
	public abstract List<SExpr> getList();

	//Only looks at child elements of this, returns a child list
	//whose first node is an atom w/ value 'key'
	public abstract SExpr find(String key);

	public abstract SExpr firstValueOf(String key);

	/* Empty vector indicates not found.  Otherwise, return.firstElement() == this
	 * 		 and return.lastElement() == <a list with *ANY* node being an atom of value 'key'>
	 * 		 and intermediate elements representing the path.
	 * 
	 * May throw IllegalStateException if called on cyclical graph.
	 * Uses BFS to return first shallowest valid SExpr.
	 */
	public abstract Vector<SExpr> recursiveFind(String key);
	public abstract Vector<SExpr>[] recursiveFindAll(String key);

	//atom value retrieval
	public abstract String value();
	public abstract int valueAsInt() throws NumberFormatException;
	public abstract long valueAsLong() throws NumberFormatException;
	public abstract double valueAsDouble() throws NumberFormatException;
	public abstract float valueAsFloat() throws NumberFormatException;
	public abstract boolean valueAsBoolean();

	//type retrieval
	public abstract boolean isAtom();
	public abstract boolean isList();
	public abstract boolean exists();
	public abstract SEXPR_TYPE type();

	//conversion to strings
	public abstract String print();
	public abstract String print(int level);	//level refers to whitespace offset
	public abstract String serialize();

	public abstract SExpr deepCopy();

	@Override
	public abstract String toString();
	
	@Override
	//DOES NOT CHECK FOR CYCLICAL TREE
	public boolean equals(Object another) {
		if (another == this)
			return true;
		if (! (another instanceof SExpr) )
			return false;
		SExpr osexpr = (SExpr) another;
		if (osexpr.type() != this.type())
			return false;
		
		if (this.isAtom()) {
			return this.value().equals(osexpr.value());
		}
		
		if (this.isList()) {
			if (osexpr.count() != this.count())
				return false;
			
			for (int i = 0; i < this.count(); ++i) {
				if (!osexpr.get(i).equals(this.get(i)))
					return false;
			}
			
			return true;
		}
		
		assert(!this.exists());
		return false;	//Somewhat arbitrary, but this says that two distinct (!=) 'Not Found' SExprs cannot be equal.
	}

	/**
	 * end functionality listing
	 * -----------------------------
	 * start implementation:
	 */

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
		public boolean isList() {
			return !atom;
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
				for (SExpr sexpr : exprs) {
					if (!sexpr.exists())
						throw new DoesNotExistException();
					
					list.add(sexpr);
				}
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
		public float valueAsFloat() throws NumberFormatException {
			return Float.parseFloat(value);
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
			throw new NumberFormatException("cannot parse value as boolean");
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

		@Override
		public SExpr firstValueOf(String key) {
			return this.find(key).get(1);
		}

		@Override
		public void setList(SExpr... items) {
			atom = false;
			value = null;
			list = new ArrayList<>(Arrays.asList(items));
		}

		@Override
		public void setList(List<SExpr> list) {
			atom = false;
			value = null;
			list = new ArrayList<>(list);
		}

		@Override
		public void setAtom(String val) {
			atom = true;
			list = null;
			value = val;
		}

		@Override
		public void insert(int index, SExpr item) {
			if (!atom && item.exists()) {
				list.add(index, item);
			}
		}

		@Override
		public boolean remove(SExpr item) {
			if (!atom && list.contains(item)) {
				list.remove(item);
				return true;
			}

			return false;
		}

		@Override
		public List<SExpr> getList() {
			return atom ? null : list;
		}

		private Vector<SExpr> traverse(Map<SExpr, SExpr> parents, SExpr start) {
			LinkedList<SExpr> ret = new LinkedList<SExpr>();
			SExpr val = start;
			while (val != null) {
				ret.addFirst(val);
				val = parents.get(val);
			}

			assert(ret.getFirst() == this);
			return new Vector<SExpr>(ret);
		}

		private ArrayList<Vector<SExpr>> internalRecursiveFind(String key, int num) {
			Map<SExpr, SExpr> parents = new HashMap<SExpr, SExpr>();
			LinkedList<SExpr> queue = new LinkedList<SExpr>();
			ArrayList<Vector<SExpr>> matches = new ArrayList<>();

			assert(!atom);
			assert(num > 0);

			queue.add(this);
			parents.put(this, null);

			while (!queue.isEmpty()) {
				SExpr current = queue.removeFirst();
				assert(!current.isAtom());

				for (SExpr child : current.getList()) {
					if (child.isAtom()) {
						if (child.value().equals(key)) {
							matches.add(traverse(parents, current));

							if (matches.size() == num) {
								return matches;
							} else {
								continue;
							}
						}
					} else {
						//list
						if (parents.containsKey(child)) {
							throw new IllegalStateException("Cyclical tree.");
						}

						parents.put(child, current);
						queue.addLast(child);
					}
				}
			}

			return matches;
		}

		@Override
		public Vector<SExpr> recursiveFind(String key) {
			if (atom || list.isEmpty())
				return new Vector<SExpr>();

			ArrayList<Vector<SExpr>> matches = internalRecursiveFind(key, 1);
			assert(matches.size() <= 1);

			if (matches.isEmpty()) {
				return new Vector<SExpr>();
			} else {
				return matches.get(0);
			}
		}

		@SuppressWarnings("unchecked")
		@Override
		public Vector<SExpr>[] recursiveFindAll(String key) {
			if (atom || list.isEmpty())
				return new Vector[0];

			ArrayList<Vector<SExpr>> matches = internalRecursiveFind(key, Integer.MAX_VALUE);
			return matches.toArray(new Vector[matches.size()]);
		}

		@Override
		public SExpr deepCopy() {
			return _deepCopy(this, new HashSet<SExpr>());
		}

		@Override
		public String toString() {
			return atom ? String.format("SExpr.atom(\"%s\")", value) : String.format("SExpr.list(%d)", list.size());
		}

		@Override
		public SEXPR_TYPE type() {
			return atom ? SEXPR_TYPE.ATOM : SEXPR_TYPE.LIST;
		}
	}

	private static class NotFound extends SExpr {

		@Override
		public boolean isAtom() {
			return false;
		}
		
		@Override
		public boolean isList() {
			return false;
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
		public float valueAsFloat() throws NumberFormatException {
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

		@Override
		public void setList(SExpr... items) {}

		@Override
		public void setAtom(String val) {}

		@Override
		public void insert(int index, SExpr item) {}

		@Override
		public boolean remove(SExpr item) {
			return false;
		}

		@Override
		public List<SExpr> getList() {
			return null;
		}

		@Override
		public void setList(List<SExpr> list) {}

		@Override
		public Vector<SExpr> recursiveFind(String key) {
			return new Vector<SExpr>();	//key not found.
		}

		@Override
		public SExpr deepCopy() {
			return NOT_FOUND;
		}

		@Override
		public String toString() {
			return "SExpr.NOTFOUND";
		}

		@Override
		public SExpr firstValueOf(String key) {
			return NOT_FOUND;
		}

		@SuppressWarnings("unchecked")
		@Override
		public Vector<SExpr>[] recursiveFindAll(String key) {
			return new Vector[0];
		}

		@Override
		public SEXPR_TYPE type() {
			return SEXPR_TYPE.NOTFOUND;
		}
	}

	//Doesn't require a throws.. or try/catch like non-Runtime exceptions.
	public static class DoesNotExistException extends RuntimeException {}

	private static class MutRef {
		int val;
		MutRef() {
			val = 0;
		}
	}

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

	private static SExpr _deepCopy(SExpr node, HashSet<SExpr> seen) {
		if (seen.contains(node)) {
			throw new IllegalStateException("Cyclical tree.");
		}

		if (node.isAtom()) {
			return atom(node.value());
		}

		//node is list, construct copy.
		seen.add(node);
		SExpr copiedList = list();
		for (SExpr child : node.getList()) {
			copiedList.append(_deepCopy(child, seen));
		}

		return copiedList;
	}
	
	public static enum SEXPR_TYPE {
		ATOM,
		LIST,
		NOTFOUND
	}

	public static void main(String[] args) {
		/*
		String ser = "(the list goes (on and on and \"what?()\" ()))";

		SExpr s = SExpr.deserializeFrom(ser);

		System.out.println("done...");
		System.out.printf("%s\n",s.print()); */

		SExpr top = list(
				list(
						atom("rqwe"),
						list()
						),
						list(
								pair("rawr", "sock"),
								pair("wa", "r"),
								atom("RAWR")
								)
				);

		System.out.println("" + top.recursiveFind("rawr"));

		for (SExpr s : top.recursiveFind("rawr")) {
			System.out.println("------------------------------------------\n"
					+ s.print());
		}

		top.append(top);
		System.out.println("" + top.recursiveFind("rawr"));
	}
}
