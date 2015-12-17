/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package bike.model;

import java.io.Serializable;

import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;

import antlr.collections.List;

/**
 *
 * @author Bartosz
 */
public interface TrainingDao extends CrudRepository<Training, Long>{
    
	@Query (value = "SELECT Count(id),sum(total_distance ) FROM TRAINING", nativeQuery = true)
	Object getStatistic();
	
}
