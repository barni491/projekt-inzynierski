package bike.model;

import com.fasterxml.jackson.annotation.JsonBackReference;
import com.fasterxml.jackson.annotation.JsonIgnore;

import java.sql.Date;
import java.sql.Timestamp;

import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;

@Entity
@Table(name = "TRAINING_POINT")
public class TrainingPoint {
	@Id
	@GeneratedValue(strategy = GenerationType.AUTO)
	private long id;
	
	private Timestamp  time;
	private Float altitude;
        
	private String longitude;
	private String latitude;
	private Float speed;
	private Float bpm;
	private Float temperature;
	private Float humidity;
	
	@ManyToOne (fetch=FetchType.EAGER )
    //  @JoinColumn(name="training" )
       //  @JsonIgnore
    @JsonBackReference
	private Training training;

    public Training getTraining() {
        return training;
    }

    public void setTraining(Training training) {
        this.training = training;
    }
	
//	Constructors, getters and setters
	public TrainingPoint(Timestamp time, Float altitude, String longitude, String latitude, Float speed, Float bpm,
			Float temperature, Float humidity) {
		super();
		this.time = time;
		this.altitude = altitude;
		this.longitude = longitude;
		this.latitude = latitude;
		this.speed = speed;
		this.bpm = bpm;
		this.temperature = temperature;
		this.humidity = humidity;
	}
        
        public TrainingPoint() {
		super();
		
	}
	
	public Timestamp getTime() {
		return time;
	}
	public void setTime(Timestamp time) {
		this.time = time;
	}
	public Float getAltitude() {
		return altitude;
	}
	public void setAltitude(Float altitude) {
		this.altitude = altitude;
	}
	public String getLongitude() {
		return longitude;
	}
	public void setLongitude(String longitude) {
		this.longitude = longitude;
	}
	public String getLatitude() {
		return latitude;
	}
	public void setLatitude(String latitude) {
		this.latitude = latitude;
	}
	public Float getSpeed() {
		return speed;
	}
	public void setSpeed(Float speed) {
		this.speed = speed;
	}
	public Float getBpm() {
		return bpm;
	}
	public void setBpm(Float bpm) {
		this.bpm = bpm;
	}
	public Float getTemperature() {
		return temperature;
	}
	public void setTemperature(Float temperature) {
		this.temperature = temperature;
	}
	public Float getHumidity() {
		return humidity;
	}
	public void setHumidity(Float humidity) {
		this.humidity = humidity;
	}
	public long getId() {
		return id;
	}
}
