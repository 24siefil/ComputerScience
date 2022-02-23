package com.example.demo.controller;

public class step1 {
	 
	   public step1() {
	       
	   }
	   public step1(Integer rating, float average_age) {
	      super();
	      this.rating = rating;
	      this.average_age = average_age;
	   }
	    
	   public Integer rating;
	   public float average_age;
	    
	   //getters and setters
	 
	   @Override
	   public String toString() {
	      return "Employee [rating=" + rating + ", average_age=" + average_age + "]";
	   }
	}
