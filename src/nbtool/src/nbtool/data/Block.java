package nbtool.data;

import nbtool.data.json.JsonObject;

public final class Block {
	/* bytes pertaining to this Block */
	public byte[] bytes;
	/* this is a REFERENCE to the relevant JsonObject in the parent Log */
	public JsonObject attributes;
	
	/* this Block's index in the enclosing Log's array of Blocks
	 * the first Block has index 0 */
	public int index;
	
	public Block(byte[] b, JsonObject o, int i) {
		this.bytes = b; this.attributes = o; this.index = i;
	}
}
