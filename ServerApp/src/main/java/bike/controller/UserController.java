package bike.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import bike.model.User;
import bike.model.UserDao;

@RestController
public class UserController {
	
	@Autowired
	private UserDao userDao;
	
	@RequestMapping(value = "/user/add", method = RequestMethod.GET)
	public String create(String name, String surname) {
	    User user = null;
	    try {
	    	user = new User(name, surname);
	    	userDao.save(user);
	    }
	    catch (Exception ex) {
	      return "Error creating the user: " + ex.toString();
	    }
	    return "User succesfully created! (id = " + user.getId() + ")";
	  }
	
	@RequestMapping(value = "/user/read", method = RequestMethod.GET)
	public String read(String name) {
	    User user = null;
	    try {
	    	user = userDao.findByName(name);
	    }
	    catch (Exception ex) {
	      return "Error reading the user: " + ex.toString();
	    }
	    return "User succesfully read! (id = " + user.getId() + ", name = " + user.getName() + ", surname = " + user.getSurname() +")";
	  }

}
