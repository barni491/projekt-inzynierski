(function(){
	
	
	'use strict';
	
	var trainingsModule = angular.module('Trainings',[]);
	
	trainingsModule.controller('trainingsController',function($scope,$http){
		
		$scope.formatDate = function(date_p){
			var date = new Date(date_p);
				return date.toLocaleDateString() +' '+ date.toLocaleTimeString();
		} ;
		
		
		$scope.time = function(sec){
			
			var minutes = Math.floor(sec/60);
			var secound = sec%60;
			return minutes+":"+("0" + secound).slice(-2) ;
		} ;
		
		$http.get('/training/all')
		.success(function(d){
			$scope.trainings = d; 
			
		}) ;
			
			
		
		
	});
	
	
})();