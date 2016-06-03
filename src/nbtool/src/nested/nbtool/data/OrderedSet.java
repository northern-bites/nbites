package nbtool.data;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

public class OrderedSet<T> {
	
	private LinkedList<T> internal = null;
	
	public OrderedSet(T[] array) {
		internal = new LinkedList<>(Arrays.asList(array));
	}
	
	public OrderedSet() {
		internal = new LinkedList<>();
	}
	
	public OrderedSet(List<T> list) {
		internal = new LinkedList<>(list);
	}
	
	public Vector<T> vector() {
		return new Vector<>(internal);
	}
	
	public Vector<T> update(T next) {
		if (internal.contains(next)) {
			internal.remove(next);
		}
		
		internal.addFirst(next);
		return vector();
	}
	
	public int index(T obj) {
		return internal.indexOf(obj);
	}
}
