<?php
global $base_url;
?>
  <div class="content" property='schema:location' typeof='schema:PerformingArtsTheater' id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?>">
    <?php
      hide($content['field_venue_website']);
      hide($content['field_venue_logo']);
      hide($content['field_metro_station_url']);
      hide($content['field_metro_lines']);
      hide($content['comments']);
      hide($content['links']);
      hide($content['field_map']);
      $link = "";
      if(isset($content['field_venue_website']) && isset($content['field_venue_website']['#object'])){
	$link = "<a ";
	$obj = $content['field_venue_website']['#object'];
	$field = array_shift(array_shift($obj->field_venue_website));
	$link .= " property='schema:sameAs'";
	$link .= " content='".$field['url']."'";
	$link .= " href='".$field['url']."'";
	$link .= " title='".$field['title']."'";
	if(is_array($field['attributes'])){
	  foreach ($field['attributes'] as $attr => $value){
	    $link .= " $attr='$value'";
	  }
	}
	$link .= ">";
      }
      echo "<div class='venue-main'>";
      if($link) print $link;
      print "<span property='schema:name' content='$title'>";
      print $title;
      print "</span>";
      if($link) print "</a>";
      print render($content['field_address']);
      if (isset($content['field_map']) &&
	  isset($content['field_map']['#object']) &&
	  isset($content['field_map']['#object']->field_map) &&
	  isset($content['field_map']['#object']->field_map['und']) &&
	  isset($content['field_map']['#object']->field_map['und'][0])){
	$coords = $content['field_map']['#object']->field_map['und'][0]['lat'].",".$content['field_map']['#object']->field_map['und'][0]['lon'];
	$placename = urlencode($title);
	print "<div class='location-map'>Map: <a href='https://www.google.com/maps/place/$placename/@$coords' target='_blank' title='Google Map Link' alt='Google Map Link' property='hasMap' content='https://www.google.com/maps/place/$placename/@$coords'><img src='$base_url/sites/all/themes/uisce/images/google-marker.png'></a></div>";
      }
      echo "<div class='metro-stop'>";
      if(isset($content['field_metro_station_url']) &&
	  isset($content['field_metro_station_url']['#object'])){
	$color_count = 0;
	print "<div class='metro-blobs'>Metro: ";
	if(isset($content['field_metro_lines'])){
	  foreach ($content['field_metro_lines']['#object']->field_metro_lines['und'] as $line){
	    $color_count += 1;
	    $color = $line['value'];
	    print "<div class='metro $color'></div>";
	  }
	}
	print "</div>";
	$station_count = count($content['field_metro_station_url']['#object']->field_metro_station_url['und']);
	if($station_count < 2 && $color_count <= 2){
	  $station = array_shift($content['field_metro_station_url']['#object']->field_metro_station_url['und']);
	  print " <a href='".$station['display_url']."' target='_blank'>".$station['title']."</a>";
	} else{
	  foreach ($content['field_metro_station_url']['#object']->field_metro_station_url['und'] as $station){
	    print "<div class='metro-links'><a href='".$station['display_url']."' target='_blank'>".$station['title']."</a></div>";
	  }
	}
      }
      echo "</div>";
      echo "</div>";
//      echo "<div class='venue-logo' id='venue-logo'>";
//      if(isset($content['field_venue_logo'])){
//        if($link) print $link;
//        print render($content['field_venue_logo']);
//	dpm($content['field_venue_logo']);
//        if($link) print "</a>";
//      }
      echo "</div>";
     ?>
  </div> <!-- /content -->
