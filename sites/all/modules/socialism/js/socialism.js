(function ($) {
  Drupal.behaviors.socialism = {
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
      $("h3.socialism-adroll-toggle").click(function(){
        var curtext = $(this).text();
        if(curtext.match(/^Get Adroll Javascript Code/)){
          $(this).html("Hide Adroll Javascript Code &#9660;");
          $("p.socialism-adroll-pixel").show(400);
          $("pre.socialism-adroll-pixel").show(400);
        } else {
          $(this).html("Get Adroll Javascript Code &#9654;");
          $("p.socialism-adroll-pixel").hide(400);
          $("pre.socialism-adroll-pixel").hide(400);
        }
      });

      $("input#edit-field-web-ad-landing-page-und-0-value, input#edit-field-facebook-ad-landing-page-und-0-value").change(function(){
        var url = $(this).val();
        if(url == null || url == ""){
          return(false);
        }
        var my_id = $(this).attr("id");
				var lookfor = /"Z6UPWBZOH5HXXDG4TJBZFV"/;
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
        $( "input#edit-field-campaigns-und-facebook" ).prop( "disabled", true );
        $("body.page-user div.field-name-field-company-website").each(function(){
          var url = $(this).find("a").attr("href");
          if(url == null || url == ""){
            return(false);
          }
				  var lookfor = /"Z6UPWBZOH5HXXDG4TJBZFV"/;
          $("span#adroll-urltest-results").html("Checking URL... <img src='/misc/throbber.gif'>")
  		    $.ajax({
      		  url: "//dctheatre.rocks/fetch-landing-url/"+encodeURIComponent(encodeURIComponent(url)),
        		complete: function(xhr, stat){
//console.log("IN HURR: "+url);
//console.log(xhr.responseText);
  						if(xhr.responseText.match(lookfor)){
                $("span#adroll-urltest-results").text("Found!");
                $("span#adroll-urltest-results").addClass("kosher");
              } else {
                $("span#adroll-urltest-results").text("Not found! See 'Get Adroll Javascript Code' on your dashboard if you need to insert it again.");
                $("span#adroll-urltest-results").addClass("no-adroll");
              }
        		},
        		error: function(xhr, stat, err){
              $("span#adroll-urltest-results").text("Failed to load page!");
              $("span#adroll-urltest-results").addClass("bad-url");
  					},
        		type: "GET"
      		});
        });
      });

    }
  }
}(jQuery));
