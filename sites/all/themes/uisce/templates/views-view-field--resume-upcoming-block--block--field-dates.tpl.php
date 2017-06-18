<?php
  print preg_replace("/>([A-Z][a-z]+) \d+, (\d{4}).*?</", ">$1 $2<", $output)
?>
