<?php

/**
 * @file
 * Available templates:
 *   simplenews-newsletter-body--[tid].tpl.php
 *   simplenews-newsletter-body--[view mode].tpl.php
 *   simplenews-newsletter-body--[tid]--[view mode].tpl.php
 * See README.txt for more details.
 *
 * Available variables:
 * - $build: Array as expected by render()
 * - $build['#node']: The $node object
 * - $title: Node title
 * - $language: Language code
 * - $view_mode: Active view mode
 * - $simplenews_theme: Contains the path to the configured mail theme.
 * - $simplenews_subscriber: The subscriber for which the newsletter is built.
 *   Note that depending on the used caching strategy, the generated body might
 *   be used for multiple subscribers. If you created personalized newsletters
 *   and can't use tokens for that, make sure to disable caching or write a
 *   custom caching strategy implemention.
 *
 * @see template_preprocess_simplenews_newsletter_body()
 */

$node = $build['#node'];

print "<p>";
if(isset($build['field_dates'])){
  $now = time();
  $valid_date_entries = Array();
  $skip = false;
  $prefix = null;
  $c = -1;
  foreach ($build['field_dates']['#items'] as $daterange){
    $c++;
    $start = strtotime($daterange['value']);
    if(!empty($daterange['value2']) and $daterange['value'] != $daterange['value2']){
      $end = strtotime($daterange['value2']);
      if($end < $now) continue;
      $valid_date_entries[] = $c;
      if($start > $now) $prefix = "Coming up";
      else if(($end - $now) < 604800) $prefix = "Final week";
      else $prefix = "Now playing";
      break;
    } else if($start > $now){
      $prefix = "One performance only";
      break;
    }
  }
//Can&#039;t Get a Decent Margarita at the North Pole
  print "<h2>".l(preg_replace("/&#039;/", "'", $title), "node/".$node->nid)."</h2>";
  foreach ($valid_date_entries as $idx){
    print "<p><div class='field-label'>".preg_replace("/\(All day\)/", "", $build['field_dates'][$idx]['#markup'])."</div></p>";
  }
}
print "</p>";

print "<p>";
if(isset($build['field_newsletter_blurb']) and !empty($build['field_newsletter_blurb'])){
  print render($build['field_newsletter_blurb']);
} else{
  print render($build['field_details']);
}
print "</p>";


print "<p>";
if(isset($build['field_tickets']) && !empty($build['field_tickets'])){
  print uisce_strip_render($build['field_tickets']);
} else{
  print "<div class='field-label'>Tickets: </div>".uisce_strip_render($build['field_company_website']);
}
print "</p>";
$result = db_query("SELECT DISTINCT delta FROM {block} WHERE bid=:bid", $args = array(':bid' => $node->field_venue['und'][0]['bid']) );
while ($delta = $result->fetchAssoc()){
  $node_block = module_invoke("nodeblock", 'block_view', $delta);
  if(isset($node_block['content'])){
    print "<p>";
    print "<div class='field-label'>Venue: </div>";
    print uisce_strip_render($node_block['content']);
    print "</p>";
  }
}

if(isset($build['field_gig_photos'][0])){
  $photos = count($build['field_gig_photos'][0]['#items']);
  for($idx = 1; $idx < $photos; $idx++){
    unset($build['field_gig_photos'][0]['#items'][$idx]);
  }
  print render($build['field_gig_photos'][0]);
}

$result = db_query("SELECT DISTINCT delta FROM {block} WHERE css_class=:css_class", $args = array(':css_class' => "block-social-media-full") );
while ($delta = $result->fetchAssoc()){
  $social_media_block = module_invoke("block", 'block_view', $delta);
  if(isset($social_media_block['content'])){
    print "<p class='social-media-links'>";
    print uisce_strip_render($social_media_block['content']);
    print "</p>";
  }
}
?>
