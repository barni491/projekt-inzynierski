
	
(function (){
	'use strict';
		var liveModule = angular.module('Live',[]);
		var training = undefined;	
		var g = undefined ;
		var temp_chart; 
		var freq_chart; 
		var speed_chart; 
		var altitude_chart;
		var current_data_id =3 ;
		var isCenter = false;
		var marker = null;
		
		
		
		liveModule.controller('liveController',function($scope,$http,$routeParams){
			$scope.humidity_data = [] ;
			$scope.temp_data = [];
			$scope.speed_data = [];
			$scope.altitude_data = [];
			$scope.freq_data = [];
			
			var trainingId = $routeParams['id'];
			$scope.altitude_opt = {
					
					drawPoints: true,
                    showRoller: true,
                    valueRange: [0, 400],
                    labels: ['Time', 'Wysokość'],
				 	highlightCallback: function(event, x, points, row, seriesName) {
				 		addMarker(points[0].idx);
			        },
			        unhighlightCallback: function(event) {
			            
			        	marker.setMap(null);
				 		
			        }
			};
			

			$scope.showBpm = true; 
			$scope.temperature = 0;
			
			angular.element(document).ready(function(){
				
				altitude_chart = new Dygraph(document.getElementById("div_g_altitude"),$scope.freq_data,$scope.altitude_opt);
				
			
				
				
			});
			
			var mapOptions = {
                zoom: 14,
                center: new google.maps.LatLng(51.108460, 17.060077),
                mapTypeId: google.maps.MapTypeId.ROADMAP
            }

            $scope.map = new google.maps.Map(document.getElementById('map'), mapOptions);
			
			var poly = new google.maps.Polyline({
				strokeColor: '#00BFFF',
				strokeOpacity: 5.0,
				strokeWeight: 6,
				clickable: false,

			});
			
			
			
			poly.setMap($scope.map);
			

			
			//var ctx = document.getElementById('myChart').getContext('2d');
			getPath();
			var timer = setInterval(function(){getPath()},5000);
			
			
			//var myNewChart = new Chart(ctx).Line(data);
			//Chart.defaults.global.animation = false;
			
			
			
			function addMarker(index){
	        	
				if(marker != null){
					marker.setMap(null);
				}

				 marker = new google.maps.Marker({
				    position:  new google.maps.LatLng(training[index].latitude,
				    		training[index].longitude),
				    
				    map: $scope.map
				 });
				
				
			}
			$scope.toggle = function(data,label,range,data_id){
				current_data_id = data_id; 
				altitude_chart.updateOptions( { 'file': data , labels: label,valueRange: range} );
				$scope.displayRpm = (data_id ==1);
				$scope.displaySpeed = (data_id ==2);
				$scope.displayAltitude = (data_id ==3);
				$scope.displayHumidity = (data_id ==4);
				$scope.displayTemperature = (data_id ==5);
			//	console.log("Helo");
				
			document.getElementById("chart_name").innerHTML = label[1];
				
			};
			function getPath(){
				var path = poly.getPath();
				
				
				$http.get('/trainingPoint/read/bytrainingid/'+trainingId)
				.success(function(d){
					path.clear();
					$scope.humidity_data = [];
					$scope.temp_data = [];
					$scope.freq_data = [];
					$scope.speed_data = [];
					$scope.altitude_data = [];
					training = d;
				//	console.log(d);
					var date2;
					
					
					//$scope.temperature = (d.totalDistance/1000).toFixed(3);
					d.forEach(function(entry){
						//data.datasets[0].data.push();
						//data.labels.push();
						date2 = new Date(entry.time);
						$scope.humidity_data.push([date2,entry.humidity]);
						$scope.temp_data.push([date2,entry.temperature]);
						$scope.freq_data.push([date2,entry.bpm]);
						
						$scope.speed_data.push([date2,entry.speed*3.6]);
						$scope.altitude_data.push([date2,entry.altitude]);
						if(entry.latitude != null ){
							path.push(new google.maps.LatLng(entry.latitude,entry.longitude));
							if(isCenter == false ){
								var center = new google.maps.LatLng(entry.latitude,entry.longitude);
								$scope.map.panTo(center);
								isCenter = true; 
							}
						};
					});
					
				});
				
				$scope.$on("$destroy", function(){
			        clearInterval(timer);
			        isCenter = false;
			    });
				
				$http.get('/training/read?id='+trainingId)
				.success(function(d){
					$scope.trainingDate = function(){
							var date = new Date(d.date);
								return date.toLocaleDateString() +' '+ date.toLocaleTimeString();
					}() ;
					$scope.duration = function(sec){
						
						var minutes = Math.floor(sec/60);
						var secound = sec%60;
						return minutes+":"+("0" + secound).slice(-2) ;
					} (d.duration);
					
					$scope.totalDistance = (d.totalDistance/1000).toFixed(3);
					$scope.avgBpm = d.avgBpm;
					$scope.avgSpeed = ((d.totalDistance / d.duration)).toFixed(2);
					
					$scope.climb = d.climb;
					$scope.downhill = d.downhill;
					
				});
				
				switch(current_data_id) {
			    case 1:
			    	altitude_chart.updateOptions( { 'file':$scope.freq_data});
			        break;
			    case 2:
			    	altitude_chart.updateOptions( { 'file':$scope.speed_data});
			        break;
			    case 3:
			    	altitude_chart.updateOptions( { 'file':$scope.altitude_data});
			        break;
			    case 4:
			    	altitude_chart.updateOptions( { 'file':$scope.humidity_data});
			        break;
			    case 5:
			    	altitude_chart.updateOptions( { 'file':$scope.temp_data});
			        break;
			    default:
			        break;
			}
				
				

			};
			
			
	
			
		});
	
	
	
})();
