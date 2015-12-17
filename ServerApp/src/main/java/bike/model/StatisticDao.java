package bike.model;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.RowMapper;

public class StatisticDao {
	@Autowired
    JdbcTemplate jdbcTemplate ;
	
	public List <Statistic> getStat(){
		
		try {
			System.out.println("What goin on" + jdbcTemplate.getDataSource().getConnection().getSchema());
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		List<Statistic> stat = jdbcTemplate.query("Select * FROM training", new RowMapper<Statistic> (){
					public Statistic mapRow(ResultSet rs, int rowNum) throws SQLException {
						Statistic statistic = new Statistic();
						System.out.println("RESULT"+rs.toString());
						statistic.setTotalDistance(rs.getFloat("total_distance"));
						return statistic;
					}
				});
		System.out.println("What" + stat.toString());
		return stat;
		
		
		
		
	}

}
