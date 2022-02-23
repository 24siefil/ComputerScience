package Hongik.ce.dbms;

import java.sql.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class App
{
	public static void main(String[] args) throws SQLException
	{
	    try (Connection conn = DriverManager.getConnection("jdbc:mariadb://localhost/MariaDB_Connection_Practice", "root", "5465")) // create connection for a server installed in localhost, with a user "root"
	    {
	        try (Statement stmt = conn.createStatement()) // create a Statement
	        {
	        	String query1 = "SELECT S.rating, AVG(S.age) AS average_age "
						+ "FROM Sailors S "
						+ "WHERE S.age >= 18 "
						+ "GROUP BY S.rating "
						+ "HAVING COUNT(*) >= 2";
	        	String query2 = "UPDATE Sailors SET age = 30.0 "
						+ "WHERE sname = 'Andy'";

	        	// execute query
	        	HashMap<Integer, Float> map_old = new HashMap();
	        	System.out.println("========== Step1 ==========");
	            try (ResultSet rs_old = stmt.executeQuery(query1))
	            {
	            	System.out.println("rating\t| average_age");
	            	System.out.println("--------+------------");
	                while (rs_old.next())
	                {
		                System.out.print(rs_old.getInt(1));
		                System.out.print("\t| ");
		                System.out.println(rs_old.getFloat(2));
		                map_old.put(rs_old.getInt(1), rs_old.getFloat(2));
		            }
	            }

	            System.out.println("\n\n========== Step2 ==========");
	            try (ResultSet dummy = stmt.executeQuery(query2))
	            {
	            	System.out.println("Successfully updated!");
	            }
	            HashMap<Integer, Float> map_new = new HashMap();
	            try (ResultSet rs_new = stmt.executeQuery(query1))
	            {
	                while(rs_new.next())
	                	map_new.put(rs_new.getInt(1), rs_new.getFloat(2));
	            }
	            System.out.print("Diff. rating: ");
	            for (Integer key : map_old.keySet())
	            {
	            	if ((float)map_old.get(key) != (float)map_new.get(key))
	            	{
	            		System.out.println(key);
	            		break ;
	            	}

	            }
	        }
	    }
	}
}
