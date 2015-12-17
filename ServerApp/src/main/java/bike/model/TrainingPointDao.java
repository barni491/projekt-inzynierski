/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package bike.model;

import java.util.List;

import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;

/**
 *
 * @author Bartosz
 */
public interface TrainingPointDao extends CrudRepository<TrainingPoint, Long> {
    
	
	public List<TrainingPoint> findByTrainingId(long Id);
  //  @Query("select t from training_point t where t.training = ?1 and t.id > ?2")
   // public List<TrainingPoint> findInRange(long Id, long startId );
}
