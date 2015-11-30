package nbtool.data.json;

import java.util.HashMap;
import java.util.Map;

import nbtool.data.json.Json.JsonValue;
import nbtool.data.json.Json.JsonValueType;
import nbtool.util.Logger;

public class JsonParser {
	
	private String text;
	public int pos;
	
	public int position() { return pos; }
	
	protected JsonParser(String text, int pos) {
		this.text = text; this.pos = pos;
	}
	
	public JsonValue parse() throws JsonParseException {
		Token tok = nextToken();
		return _parse(tok);
	}
	
	public static class JsonParseException extends Exception {
		public String parseText;
		public int pos;
		
		protected JsonParseException(String reason, int pos, String text) {
			super(reason);
			
			this.parseText = text;
			this.pos = pos;
		}
	}
	
	public class UnexpectedTokenException extends JsonParseException {
		protected UnexpectedTokenException(Token problem) {
			super(String.format("unexpected token %s at %d", problem, problem.start),
					problem.start, text);
		}
	}
	
	public class MissingTokenException extends JsonParseException {
		protected MissingTokenException(TokenType wanted, Token found) {
			super(String.format("missing token %s at %d", wanted.describe(), found.start),
					found.start, text);
		}
	}
	
	public class UnexpectedCharacterException extends JsonParseException {
		public char unexpected;
		protected UnexpectedCharacterException(char c, int pos) {
			super(String.format("unexpected character '%c' at %d", c, pos),
					pos, text);
			this.unexpected = c;
		}
	}
	
	public class MissingCharacterException extends JsonParseException {
		public char expected;
		protected MissingCharacterException(char c, int pos) {
			super(String.format("expected character '%c' at %d", c, pos),
					pos, text);
			this.expected = c;
		}
	}
	
	public static enum TokenType {
		ARRAY_START('['),
		ARRAY_END(']'),
		SEPARATOR(','),
		
		OBJECT_START('{'),
		OBJECT_DIVIDER(':'),
		OBJECT_END('}'),
		
		/* parsed trings may be delimited with the single quote ' as well
		 * but Json objects serialized using this library will always delimit with " */
		STRING('"'),
		NUMBER(null);
		
		public final Character CHARACTER;
		private TokenType(Character c) {
			this.CHARACTER = c;
		}
		
		public String describe() {
			return String.format("type-%s std_char-'%s'", toString(), "" + CHARACTER);
		}
	}
	
	private static class Token {
		TokenType type;
		String val;
		
		int start;
		int after;
		
		@Override
		public String toString() {
			return String.format("[%s{%s}{%d,%d}]",
				type, val, start, after);
		}
	}
	
	private static final Map<Character, TokenType> SPECIAL = new HashMap<>();
	static {
		SPECIAL.put('[', TokenType.ARRAY_START);
		SPECIAL.put(']', TokenType.ARRAY_END);
		SPECIAL.put(',', TokenType.SEPARATOR);
		SPECIAL.put('{', TokenType.OBJECT_START);
		SPECIAL.put(':', TokenType.OBJECT_DIVIDER);
		SPECIAL.put('}', TokenType.OBJECT_END);
		
		SPECIAL.put('\'', TokenType.STRING);
		SPECIAL.put('"', TokenType.STRING);
	}
	
	private static final Map<String, JsonValue> RESERVED = new HashMap<>();
	static {
		RESERVED.put("null", Json.NULL_VALUE);
		RESERVED.put("true", JsonBoolean.TRUE);
		RESERVED.put("false", JsonBoolean.FALSE);
	}
	
	private JsonValue _parse(Token tok) throws JsonParseException {
		switch(tok.type) {
		case ARRAY_END: {
			throw new UnexpectedTokenException(tok);
		}
		
		case ARRAY_START: {
			JsonArray array = new JsonArray();
			
			Token first = peekToken();
			if (first.type == TokenType.ARRAY_END) {
				nextToken();	//Consume ARRAY_END
				return array;
			}
			
			for(;;) {
				Token value = nextToken();
				array.add( _parse(value) );

				Token after = nextToken();				
				if (after.type == TokenType.ARRAY_END)
					break;
				if (after.type != TokenType.SEPARATOR)
					throw new MissingTokenException(TokenType.SEPARATOR, after);
			}
			
			return array;
		}
		
		case NUMBER: {
			return new JsonNumber(tok.val);
		}
		
		case OBJECT_DIVIDER: {
			throw new UnexpectedTokenException(tok);
		}
		
		case OBJECT_END: {
			throw new UnexpectedTokenException(tok);
		}
		
		case OBJECT_START: {
			JsonObject object = new JsonObject();
			
			Token first = peekToken();
			if (first.type == TokenType.OBJECT_END) {
				//Logger.println("emtpy object");
				nextToken();	//Consume OBJECT_END
				return object;
			}
			
			for (;;) {
				readObjectPair(object);
				
				Token after = nextToken();
				if (after.type == TokenType.OBJECT_END)
					break;
				if (after.type != TokenType.SEPARATOR)
					throw new MissingTokenException(TokenType.SEPARATOR, after);
			}
			
			return object;
		}
		
		case SEPARATOR: {
			throw new UnexpectedTokenException(tok);
		}
		
		case STRING: {
			if (tok.val.trim().isEmpty()) {
				throw new JsonParseException("empty STRING token: " + tok, tok.start, text);
			}
			
			if (RESERVED.containsKey(tok.val)) {
				return RESERVED.get(tok.val);
			}
			
			String string = tok.val;
			char start = string.charAt(0);
			if (SPECIAL.containsKey(start)) {
				assert(SPECIAL.get(start) == TokenType.STRING);
				assert(string.endsWith("" + start));
				
				string = string.substring(1, string.length() - 1);
			}
			
			return new JsonString(string);
		}
			
		default:
			throw new JsonParseException("unknown token " + tok, tok.start, text);
		}		
	}
	
	private void readObjectPair(JsonObject object) throws JsonParseException {
		Token keyTok = nextToken();
		JsonValue key = _parse(keyTok);
		if (key.type() != JsonValueType.STRING)
			throw new JsonParseException("Object key MUST be string", keyTok.start, text);

		Token divTok = nextToken();
		if (divTok.type != TokenType.OBJECT_DIVIDER)
			throw new MissingTokenException(TokenType.OBJECT_DIVIDER, divTok);
		
		Token valTok = nextToken();
		JsonValue value = _parse(valTok);
		object.put( (JsonString) key, value);
	}
	
	/*
	 * Consumes whitespace leading up to next token but does not consume token itself.
	 * */
	private Token peekToken() throws JsonParseException {
		for (; pos < text.length() && isWhitespace(pos); ++pos);
		
		if (pos >= text.length() ||
				text.charAt(pos) == '\0') {
			throw new JsonParseException("expected token but reached end of input", pos, text);
		}
		
		char c = text.charAt(pos);
		TokenType type = SPECIAL.get(c);
		
		if ( type != null ) {
			if (type != TokenType.STRING) {
				Token ret = new Token();
				ret.type = type;
				ret.val = null;
				ret.start = pos;
				ret.after = pos + 1;
			
				return ret;
			} else {
				//Escaped string.
				int end = pos + 1;
				for (; end < text.length() &&
					   !stringTerminated(end, c); ++end
					   );
				if ( end >= text.length() )
					throw new MissingCharacterException(c, end);
				assert(text.charAt(end) == c);
				
				Token ret = new Token();
				ret.type = TokenType.STRING;
				//Must include strchar! Otherwise "null" cannot be differentiated from null .
				ret.val = text.substring(pos, end + 1);
				ret.start = pos;
				ret.after = end + 1;
				
				return ret;
			}
		}
		
		//Number token – all numbers start with either a minus or a digit.
		if (c == '-' || Character.isDigit(c)) {
			int after = pos + 1;
			for (; after < text.length() && isNumberChar(after); ++after);
			
			Token ret = new Token();
			ret.type = TokenType.NUMBER;
			ret.val = text.substring(pos, after);
			ret.start = pos;
			ret.after = after;
			
			return ret;
		}
		
		//Unescaped string (or possibly reserved string)
		if (Character.isLetter(c)) {
			int after = pos + 1;
			for (; after < text.length() &&
				   !isWhitespace(after) &&
				   !SPECIAL.containsKey(text.charAt(after)); ++after);
			
			Token ret = new Token();
			ret.type = TokenType.STRING;
			ret.val = text.substring(pos, after);
			ret.start = pos;
			ret.after = after;
			
			return ret;
		}
		
		throw new UnexpectedCharacterException(c, pos);
	}
	
	private Token nextToken() throws JsonParseException {
		Token ret = peekToken();
		pos = ret.after;
		return ret;
	}
	
	private boolean isWhitespace(int i) {
		char c = text.charAt(i);
		return Character.isWhitespace(c);
	}
	
	private boolean isNumberChar(int i) {
		char c = text.charAt(i);
		return Character.isLetterOrDigit(c) || c == '.';
	}
	
	private boolean stringTerminated(int p, char strchar) {
		return text.charAt(p) == strchar && text.charAt(p - 1) != '\\';
	}
	
	
	/*
	 * For immediate testing – long term testing must be done in the unit tests ( nbtool.term.units )
	 * */
	
	public static void main(String[] args) throws JsonParseException {
		/*
		String line = "[hello,null,true,{}, \"hello there\", {val:45}]";
		JsonParser parser = new JsonParser(line, 0);
		Logger.println("go....");
		
		JsonValue val = parser.parse();
		Logger.println("VALUE:" + val);
		
		JsonObject obj = new JsonObject();
		obj.put("key", new JsonString("value"));
		obj.put("array", new JsonNumber("5000"));
		obj.put("reuse", new JsonObject());
		String serd = obj.print();
		
		JsonValue val2 = Json.parse(serd);
		Logger.printf("--------------------------------\n%s", val2.print()); */
		
		/*
		JsonArray array = new JsonArray();
		array.add(new JsonString("fellow"));
		array.add(new JsonString("cow"));
		array.add(new JsonNumber("5000"));
		array.add(new JsonString("thing"));
		
		JsonObject obj = new JsonObject();
		obj.put("key1", new JsonNumber("50"));
		obj.put("key2", new JsonString("the string"));
		array.add(obj);
		Logger.println(array.print() + "\n"); */
		
		JsonArray outer = new JsonArray();
		JsonArray inner = Json.array();
		
		outer.add(Json.NULL_VALUE);
		inner.add(Json.NULL_VALUE);
		outer.add(inner);
		
		Logger.println(outer.print() + "\n");
	}
}
