// B617065 신성우

package com.example.demo.controller;
 
import java.util.ArrayList;
import java.util.List;

import java.sql.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class HomeController 
{	
	@RequestMapping(value="/step1") //http://localhost:8000/step1
	public List<step1> getStep1() throws SQLException
	{
		List<step1> step1List = new ArrayList<step1>();
		
		 try (Connection conn = DriverManager.getConnection("jdbc:mariadb://localhost/MariaDB_Connection_Practice", "root", "5465")) // create connection for a server installed in localhost, with a user "root"
		    {
		        try (Statement stmt = conn.createStatement()) // create a Statement
		        {
		        	String query1 = "SELECT S.rating, AVG(S.age) AS average_age "
							+ "FROM Sailors S "
							+ "WHERE S.age >= 18 "
							+ "GROUP BY S.rating "
							+ "HAVING COUNT(*) >= 2";

		        	// execute query
		        	HashMap<Integer, Float> map_old = new HashMap();
		            try (ResultSet rs_old = stmt.executeQuery(query1))
		            {
		                while (rs_old.next())
		                {
		                	step1List.add(new step1(rs_old.getInt(1), rs_old.getFloat(2)));
			                map_old.put(rs_old.getInt(1), rs_old.getFloat(2));
			            }
		            }
		        }
		    }		
		return step1List;
	}
	
	/*
  @RequestMapping(value="/")
  public String index() {
      
      return "index";
  }
  */
	@RequestMapping(value="/employee") //http://localhost:8000/employee
	 public List<Employee> getEmployees() 
	    {
	      List<Employee> employeesList = new ArrayList<Employee>();
	      employeesList.add(new Employee(1,"sungwoo","shin","virtuoso.swshin3153@gmail.com"));
	      return employeesList;
	    }
}
