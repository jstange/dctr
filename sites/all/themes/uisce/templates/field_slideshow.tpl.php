<?php
/**
 * @file
 * Template file for field_slideshow
 *
 *
 */

// Should fix issue #1502772
// @todo: find a nicer way to fix this
if (!isset($controls_position)) {
  $controls_position = "after";
}
if (!isset($pager_position)) {
  $pager_position = "after";
}
?>
<div id="field-slideshow-<?php print $slideshow_id; ?>-wrapper" class="field-slideshow-wrapper">

  <?php if ($controls_position == "before")  print(render($controls)); ?>

  <?php if ($pager_position == "before")  print(render($pager)); ?>

  <?php if (isset($breakpoints) && isset($breakpoints['mapping']) && !empty($breakpoints['mapping'])) {
    $style = 'height:' . $slides_max_height . 'px';
  } else {
    $style = 'width:' . $slides_max_width . 'px; height:' . $slides_max_height . 'px';
  } ?>

  <div class="<?php print $classes; ?>" style="<?php print $style; ?>">
    <?php foreach ($items as $num => $item) : ?>
      <div class="<?php print $item['classes']; ?>"<?php if ($num) : ?> style="display:none;"<?php endif; ?>>
        <?php print $item['image']; ?>
        <div class="field-slideshow-caption">
        <?php if (isset($item['caption']) && $item['caption'] != '') : ?>
            <span class="field-slideshow-caption-text"><?php print $item['caption']; ?></span>
        <?php endif; ?>

        <span class="field-slideshow-misc-text">
    <?php
      if(isset($item) && is_array($item) && isset($item['field_gig_photo_credit']) && is_array($item['field_gig_photo_credit'])){
	$credits = array_shift($item['field_gig_photo_credit']);
	if(is_array($credits)){
	$credit = array_shift($credits);
	  print "<div class='photo-credit'>";
	  if(isset($credit['title']) && !empty($credit['title'])){
	    print "Photo by ";
	    if(!empty($credit['url'])){
	      print "<a href='".$credit['url']."' target='_blank'>";
	    }
	    if(!empty($credit['title'])) print $credit['title'];
	    if(!empty($credit['url'])) print "</a>";
	  }
	  print "</div>";
	}
      }
      if(isset($item['field_gig_photo_credit']) && is_array($item['field_gig_photo_also_pictured'])){
	$alsos = array_shift($item['field_gig_photo_also_pictured']);
	$also_names = Array();
	print "<div class='photo-also-pictured'>";
	if(is_array($alsos)){
	  foreach ($alsos as $also) $also_names[] = $also['safe_value'];
	  if(count($also_names) > 0){
	    print "Also pictured: ".implode(", ", $also_names);
	  }
	}
	print "</div>";
      }
    ?>

	    </span>
          </div>
      </div>
    <?php endforeach; ?>
  </div>

  <?php if ($controls_position != "before") print(render($controls)); ?>

  <?php if ($pager_position != "before") print(render($pager)); ?>

</div>
