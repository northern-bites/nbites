package nbtool.test;

import javax.swing.tree.TreePath;

public class olcode {

	/*protected void tryIncrementSelection() {
		TreePath path = tree.getSelectionPath();
		if (path == null || path.getPathCount() != 3)
			return;
		
		int index = this.getIndexOfChild(path.getPathComponent(1), path.getPathComponent(2));
		int size = this.getChildCount(path.getPathComponent(1));
		
		if (!(index + 1 < size))
			return;
		Object[] newsela = path.getPath();
		newsela[2] = this.getChild(newsela[1], index + 1);
		
		tree.setSelectionPath(new TreePath(newsela));
	}
	
	protected void tryDecrementSelection() {
		TreePath path = tree.getSelectionPath();
		if (path == null || path.getPathCount() != 3)
			return;
		
		int index = this.getIndexOfChild(path.getPathComponent(1), path.getPathComponent(2));
		
		if (!(index - 1 >= 0))
			return;
		Object[] newsela = path.getPath();
		newsela[2] = this.getChild(newsela[1], index - 1);
		
		tree.setSelectionPath(new TreePath(newsela));
	}
	 * */
	
	
	/*
	 * public void changeSelection(int direction) {
		
		if (direction > 0) {
			model.tryIncrementSelection();
			return;
		}
		
		if (direction < 0) {
			model.tryDecrementSelection();
			return;
		}
	}
	 * */
	
}
