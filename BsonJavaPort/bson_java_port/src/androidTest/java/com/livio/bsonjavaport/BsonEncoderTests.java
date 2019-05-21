package com.livio.bsonjavaport;

import com.livio.BSON.BsonEncoder;

import junit.framework.TestCase;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Random;
import java.util.Set;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class BsonEncoderTests extends TestCase {

	private HashMap<String, Object> testMapA;
	private HashMap<String, Object> testMapB;
	private HashMap<String, Object> testMapC;
	private byte[] testMapAbytes, testMapBbytes;

	public void setUp() throws Exception{
		super.setUp();

		testMapA = new HashMap<>();
		testMapA.put("hello","world");

		testMapAbytes = hexStringToByteArray(
				"16000000" +
						"02" +
						"68656c6c6f" +"00" + // hello
						"06000000" + "776f726c64" + "00" + //world
						"00"
		);

		testMapB = new HashMap<>();
		ArrayList<Object> list = new ArrayList<>();
		list.add("awesome");
		list.add(5.05);
		list.add(1986);
		testMapB.put("BSON", list);

		testMapBbytes = hexStringToByteArray(
				"31000000" +
						"04" + "42534f4e" + "00" + //BSON
						"26000000" +
						"02300008000000" + "617765736f6d65" +"00" + // awesome
						"0131003333333333331440" +
						"103200c2070000" +
						"00" +
						"00"
		);

		testMapC = new HashMap<>();

		HashMap<String, Object> map2 = new HashMap<>();
		HashMap<String, Object> map3 = new HashMap<>();
		ArrayList<Object> list2 = new ArrayList<>();
		ArrayList<Object> list3 = new ArrayList<>();

		testMapC.put("correct", true);
		testMapC.put("one", 64);
		testMapC.put("two", 2.5);
		testMapC.put("chars", "aaaaaaaaaaaa");

		map2.put("a", 604);
		map2.put("b", "AnotherString");
		map2.put("c", 2.45);

		testMapC.put("MapTest", map2);

		list2.add(23);
		list2.add(5.4);
		list2.add("A string");

		map3.put("i", 64);
		map3.put("Test", 4);

		list3.add(235);
		list3.add(5.54);
		list3.add("AString");

		map3.put("secondarray", list3);

		list2.add(map3);

		testMapC.put("ArrayTest", list2);
	}

	public void testEncoding(){
		byte[] observedMapAbytes = BsonEncoder.encodeToBytes(testMapA);
		for(int i = 0; i < observedMapAbytes.length; i++){
			try {
				assertEquals(observedMapAbytes[i], testMapAbytes[i]);
			}catch (Exception e){
				fail();
			}
		}

		byte[] observedMapBbytes = BsonEncoder.encodeToBytes(testMapB);
		for(int i = 0; i < observedMapBbytes.length; i++){
			try {
				assertEquals(observedMapBbytes[i], testMapBbytes[i]);
			}catch (Exception e){
				fail();
			}
		}
	}

	public void testDecoding(){
		HashMap<String, Object> decodedMapA = BsonEncoder.decodeFromBytes(testMapAbytes);
		HashMap<String, Object> decodedMapB = BsonEncoder.decodeFromBytes(testMapBbytes);

		assertTrue(compareHashMaps(testMapA, decodedMapA));
		assertTrue(compareHashMaps(testMapB, decodedMapB));
	}

	public void testDecodingRandomData() {
		// Checking for proper handling of invalid data
		byte[] randomBytes = new byte[200];
		new Random().nextBytes(randomBytes);
		BsonEncoder.decodeFromBytes(randomBytes);
	}

	public void testEncodeDecodeConsistency() {
		// Test nested objects and arrays
		byte[] bytes = BsonEncoder.encodeToBytes(testMapC);
		HashMap<String, Object> outMap = BsonEncoder.decodeFromBytes(bytes);
		assertEquals(testMapC, outMap);
	}

	private boolean compareHashMaps(HashMap<String,Object> testMap, HashMap<String,Object> obsvMap){
		Set<Map.Entry<String, Object>> observedEntrySet = obsvMap.entrySet();
		Set<Map.Entry<String, Object>> testEntrySet = testMap.entrySet();

		Iterator<Map.Entry<String, Object>> obsv_it = observedEntrySet.iterator();
		Iterator<Map.Entry<String, Object>> test_it = testEntrySet.iterator();
		while(obsv_it.hasNext() && test_it.hasNext()){
			Map.Entry<String, Object> obsv = obsv_it.next();
			Map.Entry<String, Object> test = test_it.next();

			if(!obsv.getKey().equals(test.getKey()))
				return false;
			if(!obsv.getValue().equals(test.getValue()))
				return false;
		}
		return !obsv_it.hasNext() && !test_it.hasNext();
	}

	// Helper method for converting String to Byte Array
	private static byte[] hexStringToByteArray(String s) {
		int len = s.length();
		byte[] data = new byte[len / 2];
		for (int i = 0; i < len; i += 2) {
			data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
					+ Character.digit(s.charAt(i+1), 16));
		}
		return data;
	}
}
