$(document).ready(function() {
	// setup timer to update statistics information every 10 seconds
	$(this).everyTime(10000, function() {
		// request information from server
		$.ajax({
			url: "statistics/current",
			cache: false,
			dataType: "xml",
			success: function(xml) {
				alert("Data received: " + xml);
				$("statistics", xml).each(function(i) {
					var uptime = $(this).find("uptime").text();
					var load = $(this).find("load").text();
					var sessions = $(this).find("sessions").text();
					var peak = $(this).find("peak").text();
				});
			}
		});
	});
});
