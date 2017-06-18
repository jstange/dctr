<?php

/**
* @file
* Default theme implementation to display a node.
*
* Available variables:
* - $title: the (sanitized) title of the node.
* - $content: An array of node items. Use render($content) to print them all,
*   or print a subset such as render($content['field_example']). Use
*   hide($content['field_example']) to temporarily suppress the printing of a
*   given element.
* - $user_picture: The node author's picture from user-picture.tpl.php.
* - $date: Formatted creation date. Preprocess functions can reformat it by
*   calling format_date() with the desired parameters on the $created variable.
* - $name: Themed username of node author output from theme_username().
* - $node_url: Direct url of the current node.
* - $display_submitted: Whether submission information should be displayed.
* - $submitted: Submission information created from $name and $date during
*   template_preprocess_node().
* - $classes: String of classes that can be used to style contextually through
*   CSS. It can be manipulated through the variable $classes_array from
*   preprocess functions. The default values can be one or more of the
*   following:
*   - node: The current template type, i.e., "theming hook".
*   - node-[type]: The current node type. For example, if the node is a
*     "Blog entry" it would result in "node-blog". Note that the machine
*     name will often be in a short form of the human readable label.
*   - node-teaser: Nodes in teaser form.
*   - node-preview: Nodes in preview mode.
*   The following are controlled through the node publishing options.
*   - node-promoted: Nodes promoted to the front page.
*   - node-sticky: Nodes ordered above other non-sticky nodes in teaser
*     listings.
*   - node-unpublished: Unpublished nodes visible only to administrators.
* - $title_prefix (array): An array containing additional output populated by
*   modules, intended to be displayed in front of the main title tag that
*   appears in the template.
* - $title_suffix (array): An array containing additional output populated by
*   modules, intended to be displayed after the main title tag that appears in
*   the template.
*
* Other variables:
* - $node: Full node object. Contains data that may not be safe.
* - $type: Node type, i.e. story, page, blog, etc.
* - $comment_count: Number of comments attached to the node.
* - $uid: User ID of the node author.
* - $created: Time the node was published formatted in Unix timestamp.
* - $classes_array: Array of html class attribute values. It is flattened
*   into a string within the variable $classes.
* - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
*   teaser listings.
* - $id: Position of the node. Increments each time it's output.
*
* Node status variables:
* - $view_mode: View mode, e.g. 'full', 'teaser'...
* - $teaser: Flag for the teaser state (shortcut for $view_mode == 'teaser').
* - $page: Flag for the full page state.
* - $promote: Flag for front page promotion state.
* - $sticky: Flags for sticky post setting.
* - $status: Flag for published status.
* - $comment: State of comment settings for the node.
* - $readmore: Flags true if the teaser content of the node cannot hold the
*   main body content.
* - $is_front: Flags true when presented in the front page.
* - $logged_in: Flags true when the current user is a logged-in member.
* - $is_admin: Flags true when the current user is an administrator.
*
* Field variables: for each field instance attached to the node a corresponding
* variable is defined, e.g. $node->body becomes $body. When needing to access
* a field's raw values, developers/themers are strongly encouraged to use these
* variables. Otherwise they will have to explicitly specify the desired field
* language, e.g. $node->body['en'], thus overriding any language negotiation
* rule that was previously applied.
*
* @see template_preprocess()
* @see template_preprocess_node()
* @see template_process()
*/
global $base_url;
if(isset($node->rdf_mapping['comment_count'])){
  unset($node->rdf_mapping['comment_count']);
}
if(isset($title_suffix->rdf_mapping['comment_count'])){
  unset($title_suffix->rdf_mapping['comment_count']);
}
//['style_plugin']['row_plugin']->nodes as $node
?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print preg_replace('/ sioc:Item foaf:Document/', "", $attributes); ?> vocab="http://schema.org/">

  <?php print render($title_prefix); ?>
  <?php
    $staging = field_get_items('node', $node, 'field_staging');
    if(isset($staging[0]['value']) && $staging[0]['value'] == 'Staged Reading'){
      $title_append = " <em>(Staged Reading)</em>";
    } else{
      $title_append = "";
    }
  ?>
  <?php if (!$page && $title): ?>
  <header>
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url ?>?width=80%25&height=80%25"  
    title="<?php print $title ?>" class="colorbox-node" property='schema:url' content='<?php print "$base_url$node_url" ?>'>
    <span property="schema:name"><?php print $title.$title_append ?></span></a></h2>
  <?php else: ?>
  <?php if ($title): ?><h1<?php print $title_attributes; ?> class="page-title" property="schema:name"><?php print $title.$title_append; ?></h1><?php endif; ?>
</header>
<?php endif; ?>
<div property="schema:workPerformed" typeof="schema:CreativeWork">
  <span property="name" content="<?php print $title; ?>"></span>
</div>
<!-- ?php print render($title_suffix); ? -->

<?php if ($display_submitted): ?>
  <div id="submit-wrapper">

    <!-- Overidden in template.php to show just username. -->
    <span class="submitted"><?php print $submitted; ?></span>

    <!-- Then show the date in parts for better theming. -->
    <div class="date-in-parts">
      <span class="day"><?php print $thedate;  ?></span>
      <span class="month"><?php print $themonth;  ?></span>
      <span class="year"><?php print $theyear;  ?></span>
    </div><!--//date-in-parts -->
  </div><!--//submit-wrapper-->
<?php endif; ?>

<?php
// We hide the comments and links now so that we can render them later.
hide($content['comments']);
hide($content['links']);
hide($content['field_roles']);
hide($content['field_when']);
hide($content['field_dates']);
hide($content['field_staging']);
hide($content['field_company_website']);
hide($content['field_festival']);
hide($content['field_tickets']);
hide($content['field_newsletter_blurb']);
hide($content['field_venue']);
hide($content['field_ticket_price']);
hide($content['field_details']);
print render($content['field_festival']);
if(isset($content['field_dates'])){
  print preg_replace("/ \(All day\)/", "", render($content['field_dates']));
} else {
  print render($content['field_when']);
}
print render($content['field_company_website']);
$ticket_link = render($content['field_tickets']);

if(!empty($ticket_link)){
  print "<div property='schema:offers' typeof='schema:Offer' class='ticket-price'>";
  print "Tickets";
  if($content['field_ticket_price'][0]['#markup']){
    print " (<span property='schema:price'>".$content['field_ticket_price'][0]['#markup']."</span>)";
  }
  print ": ".$ticket_link;
  print "</div>";
}
//dpm($content['field_ticket_price'][0]['#markup']);
//$content['field_tickets'][0]['#field']['label'] .= " (".$content['field_ticket_price'][0]['#markup'].")";
//$content['field_tickets']['#title'] .= " (".$content['field_ticket_price'][0]['#markup'].")";
//dpm($content['field_tickets']);
//print render($content['field_tickets']);
print render($content['field_director']);

// Output our Roles collection in a reasonable format
$items = field_get_items('node', $node, 'field_roles');
$wrapper = entity_metadata_wrapper('node', $node);
$count = 0;
$do_newlines = false;
print "<p class='field field-name-field-roles'>";
foreach ($wrapper->field_roles as $entry){
  $count++;
  $name_field = $entry->field_role_name->value();
  $name = $name_field['value'];
  $show_field = $entry->field_show_name->value();
  $show = $show_field['value'];
  if(!empty($show)) $do_newlines = true;
  $is_understudy = false;
  if(isset($entry->field_understudy)){
    if($entry->field_understudy->value() == "1"){
      $is_understudy = true;
    }
  }
  print "<strong>$name</strong>";
  if(!empty($show)) print " in <em>$show</em>";
  if($is_understudy) print " (Understudy)";
  if($count != count($wrapper->field_roles) && count($wrapper->field_roles) > 2) print ", ";
  if($do_newlines) print "<br />";
  else if($count+1 == count($wrapper->field_roles)) print " and ";
}
if ($count > 0){
  print "<span class='hidden' property='performers'>John Stange</span>";
}
print "</p>";
print render($content['field_details']);
print render($content['field_venue']);
?>

<!-- ?php if ($node_block): ?-- >
  <!-- div id="node-block" -->
    <!-- ?php print render($node_block); ? -->
  <!-- /div -->
<!-- ?php endif; ? -->

</div>
