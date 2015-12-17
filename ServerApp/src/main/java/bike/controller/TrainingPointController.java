/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package bike.controller;



import bike.model.StatisticDao;
import bike.model.TrainingDao;
import bike.model.TrainingPoint;
import bike.model.TrainingPointDao;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.RestController;

/**
 *
 * @author Bartosz
 */

@RestController
public class TrainingPointController {
    
    @Autowired
    TrainingPointDao trainingPointDao;
    @Autowired
    TrainingDao trainingDao;
    
    
    
    @RequestMapping(value = "/trainingPoint/add",consumes="application/json", method = RequestMethod.POST)
    public String createTrainingPoint(@RequestBody TrainingPoint trainingPoint)
    {
        System.out.println("Downhill" + trainingPoint.getTraining().getDownhill());
       try{ 
    	   
    	trainingDao.save(trainingPoint.getTraining());
        trainingPointDao.save(trainingPoint);
        return "TrainingPoint added";
       }
       catch (Exception ex){
           return "Error creating TrainingPoint";
       }
       
    }
    
    @RequestMapping(value = "/trainingPoint/read/{id}", method = RequestMethod.GET)
    public @ResponseBody TrainingPoint getTrainingPoint(@PathVariable long id)
    {   
       TrainingPoint trainingPoint = null;
       try{ 
       trainingPoint =  trainingPointDao.findOne(id);
       }
       catch (Exception ex){
    	   return null;
       }
       return trainingPoint;
    }
    
     @RequestMapping(value = "/trainingPoint/read/bytrainingid/{id}", method = RequestMethod.GET)
    public @ResponseBody List<TrainingPoint> getTrainingPointById(@PathVariable long id)
    {   
       List<TrainingPoint> trainingPoint = null;
       try{ 
       trainingPoint =  trainingPointDao.findByTrainingId(id);
       }
       catch (Exception ex){
    	   return null;
       }
       return trainingPoint;
    }
    
}
