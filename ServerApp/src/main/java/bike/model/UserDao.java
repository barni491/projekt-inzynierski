package bike.model;

import org.springframework.data.repository.CrudRepository;

import bike.model.User;

public interface UserDao extends CrudRepository<User, Long>{
	public User findByName(String name);
}