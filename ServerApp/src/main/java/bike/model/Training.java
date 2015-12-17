package bike.model;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonManagedReference;
import java.sql.Timestamp;
import java.util.Set;

import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedEntityGraph;
import javax.persistence.OneToMany;
import javax.persistence.OrderBy;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;



@Entity
//@NamedEntityGraph(name = "GroupInfo.detail", attributeNodes = @NamedAttributeNode("trainingPoints"))
@Table(name = "TRAINING")
public class Training {
	@Id
	private long id;
	
	private Timestamp date;
	private Float avgSpeed;
	
	private Float totalDistance;
	private Float duration;
	private Integer climb;
	private Integer downhill;
	private Float avgBpm;
	
	@OneToMany(mappedBy="training")
    //    @JsonManagedReference
    @OrderBy("id ASC")
	private Set<TrainingPoint> trainingPoints;
	
//	Constructors, getters and setters
	public Training(Timestamp date, Float avgSpeed, Float totalDistance, Float duration, Integer climb, Integer downhill,
			Float avgBpm) {
		super();
		this.date = date;
		this.avgSpeed = avgSpeed;
		
		this.totalDistance = totalDistance;
		this.duration = duration;
		this.climb = climb;
		this.downhill = downhill;
		this.avgBpm = avgBpm;
	}
        
        public Training() {
		
	}
        
	public Training(Timestamp date) {
		this.date = date;
	}
	public Timestamp getDate() {
		return date;
	}
	public void setDate(Timestamp date) {
		this.date = date;
	}
	public Float getAvgSpeed() {
		return avgSpeed;
	}
	public void setAvgSpeed(Float avgSpeed) {
		this.avgSpeed = avgSpeed;
	}
	public Float getTotalDistance() {
		return totalDistance;
	}
	public void setTotalDistance(Float totalDistance) {
		this.totalDistance = totalDistance;
	}
	public Float getDuration() {
		return duration;
	}
	public void setDuration(Float duration) {
		this.duration = duration;
	}
	public Integer getClimb() {
		return climb;
	}
	public void setClimb(Integer climb) {
		this.climb = climb;
	}
	public Integer getDownhill() {
		return downhill;
	}
	public void setDownhill(Integer downhill) {
		this.downhill = downhill;
	}
	public Float getAvgBpm() {
		return avgBpm;
	}
	public void setAvgBpm(Float avgBpm) {
		this.avgBpm = avgBpm;
	}
	@JsonBackReference
	public Set<TrainingPoint> getTrainingPoints() {
		return trainingPoints;
	}
	public void setTrainingPoints(Set<TrainingPoint> trainingPoints) {
		this.trainingPoints = trainingPoints;
	}
	public long getId() {
		return id;
	}	
}
