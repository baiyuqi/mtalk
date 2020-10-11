import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class Regex {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		 Pattern p = Pattern.compile("(^\\s*)(command)(\\s*):(\\s*)((picture)|(location)|(profile)|(callback))(\\s*$)");

	     Matcher m = p.matcher(" 		command 	: 	picture 		");
	     System.out.print(m.matches());
	     String reportPattern = "(^\\s*)((picture)|(location)|(profile))(\\s+)(mobileId)(\\s*):(\\s*)[0-9]+\\s+.+";
	     p = Pattern.compile(reportPattern);
	    m = p.matcher(" picture mobileId : 13311509083 http");
	    System.out.print(m.matches());
	    
	    String picturePattern = "(^\\s*)((picture)|(location)|(profile))(\\s+)(mobileId)(\\s*):(\\s*)[0-9]+\\s+(url)\\s*=\\s*.+";
	    p = Pattern.compile(picturePattern);
	    m = p.matcher(" picture mobileId : 13311509083 url  =  http:1111");
	    System.out.print(m.matches());
	    String locationPattern = "(^\\s*)((picture)|(location)|(profile))(\\s+)(mobileId)(\\s*):(\\s*)[0-9]+\\s+(longitude)\\s*=\\s*[0-9]+\\.[0-9]+\\s+(latitude)\\s*=\\s*[0-9]+\\.[0-9]+\\s*";
	    p = Pattern.compile(locationPattern);
	    m = p.matcher(" picture mobileId : 13311509083 longitude  = 100.28 latitude =44.35");
	    System.out.print(m.matches());
	}

}
