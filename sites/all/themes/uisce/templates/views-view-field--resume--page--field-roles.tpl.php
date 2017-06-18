<?php

/*
 * This formats the output of our Role list (a Field Collection) for the Views
 * resume display.  Keep it short and simple- print the first one that looks
 * correct, note with "et al" if there are more, and move on.
*/

if(!empty($row)){
  $node = node_load($row->nid);
  $items = field_get_items('node', $node, 'field_roles');
  $wrapper = entity_metadata_wrapper('node', $node);
  $roles = Array();
  foreach ($wrapper->field_roles as $role){
    $name_field = $role->field_role_name->value();
    if(empty($name_field['value'])) continue;
    print $name_field['value'];
    if(count($wrapper->field_roles) > 1) print " et al";
    $is_us = $role->field_understudy->value();
    if(!empty($is_us)) print " (US)";
    break;
  }
}

?>
