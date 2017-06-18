<?php

/*
 * Stick (SR) next to things that're staged readings
*/

print $output;
if(!empty($row)){
  $node = node_load($row->nid);
  $staging = field_get_items('node', $node, 'field_staging');
  if($staging[0]['value'] == "Staged Reading"){
    print " <span style='font-style:normal;'>(SR)</span>";
  }
}

?>
