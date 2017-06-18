(function ($) {
  Drupal.behaviors.communism = {
    attach: function (context) {

      function setEstimatedCampaignCost(){
        var multiplier = 0
        var bid = $('#edit-field-bid-und-value-value').val();
        if ($('input#edit-field-campaigns-und-facebook').is(":checked")){
          multiplier++;
          $('fieldset.group-facebook').show(400);
        } else {
          $('fieldset.group-facebook').hide(400);
        }
        if ($('input#edit-field-campaigns-und-web').is(":checked")){
          multiplier++;
          $('fieldset.group-web').show(400);
        } else {
          $('fieldset.group-web').hide(400);
        }
        if (bid == null || bid == ""){
          return;
        }
        var d1_arr = $('#edit-field-dates-und-0-value-datepicker-popup-0').val().split("/");
        date1 = new Date(d1_arr[2], d1_arr[0]-1, d1_arr[1]).getTime();
        var d2_arr = $('#edit-field-dates-und-0-value2-datepicker-popup-0').val().split("/");
        date2 = new Date(d2_arr[2], d2_arr[0]-1, d2_arr[1]).getTime();
        var difference_ms = date2 - date1;
        var weeks = difference_ms/604800000;
        if (Math.round(weeks) != weeks){
          weeks = weeks.toFixed(1);
        }
        $('#campaign-startup').text((bid*multiplier).toFixed(2));
        $('#campaign-weeks').text(weeks);
        	$('#campaign-upkeep').text((weeks*bid*multiplier).toFixed(2));
        $('#campaign-total').text((weeks*bid*multiplier + bid*multiplier).toFixed(2));
      }

/*
$(document).ajaxComplete(function(e, xhr, settings) {
console.log(xhr);
});
*/

      $("input#edit-field-web-ad-landing-page-und-0-value, input#edit-field-facebook-ad-landing-page-und-0-value").change(function(){
        var url = $(this).val();
        var my_id = $(this).attr("id");
				var lookfor = /adroll_adv_id = "Z6UPWBZOH5HXXDG4TJBZFV"/;
				if(!url.match(/^https?:\/\//)){
					url = "http://"+url;
				}
        $("div."+my_id+"-msg").html("Checking URL... <img src='/misc/throbber.gif'>");
		    $.ajax({
    		  url: "//dctheatre.rocks/fetch-landing-url/"+encodeURIComponent(encodeURIComponent(url)),
      		complete: function(xhr, stat){
            $("input#"+my_id+", div."+my_id+"-msg").removeClass("bad-url");
            $("input#"+my_id+", div."+my_id+"-msg").removeClass("kosher");
            $("input#"+my_id+", div."+my_id+"-msg").removeClass("no-adroll");
						if(xhr.responseText.match(lookfor)){
              $("input#"+my_id+", div."+my_id+"-msg").addClass("kosher");
              $("div."+my_id+"-msg").text("URL looks good!");
            } else {
              $("input#"+my_id+", div."+my_id+"-msg").addClass("no-adroll");
              $("div."+my_id+"-msg").text("I can't find the Adroll JavaScript code on that page!");
            }
      		},
      		error: function(xhr, stat, err){
            $("input#"+my_id+", div."+my_id+"-msg").addClass("bad-url");
            $("div."+my_id+"-msg").text("Got a bad response trying to check that page!");
					},
      		type: "GET"
    		});
      });

      $(document).ready(function(){
        setEstimatedCampaignCost();
        $('#edit-field-bid-und-value-value, input#edit-field-campaigns-und-facebook, input#edit-field-campaigns-und-web, #edit-field-dates-und-0-value-datepicker-popup-0, #edit-field-dates-und-0-value2-datepicker-popup-0').change(function() {
          setEstimatedCampaignCost();
        });
      });

    }
  }
}(jQuery));
