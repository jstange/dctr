<?php
/**
 * @file
 * Drush integration for Adroll.
 */

function socialism_drush_command(){
  $items['finish-campaigns'] = array(
    'description' => "Look for campaigns that have completed in Adroll, but do not have associated campaign reports, and generate those reports.",
    'aliases' => array('sfc')
  );
  $items['check-for-stuck-campaigns'] = array(
    'description' => "Look for campaigns that are ostensibly running in Adroll, but have not served any impressions.",
    'aliases' => array('csc')
  );
  $items['check-for-unlaunched-campaigns'] = array(
    'description' => "Look for campaigns that are overdue to run, but have not yet been launched.",
    'aliases' => array('cuc')
  );
  $items['list-contacts'] = array(
    'description' => "Rattle off email addresses of everyone who uses the thing.",
    'aliases' => array('slc')
  );
  return $items;
}

function drush_socialism_list_contacts(){
  $users = entity_load('user');  
  $emails = array();
  foreach ($users as $u){
    if(!isset($u->field_email_contacts[LANGUAGE_NONE])){
      continue;
    }
    foreach ($u->field_email_contacts[LANGUAGE_NONE] as $email){
      $emails[] = $email['email'];
    }
  }
  print implode(", ", array_unique($emails))."\n";
}

function drush_socialism_check_for_unlaunched_campaigns(){
  _socialism_find_unlaunched_campaigns();
}

function drush_socialism_check_for_stuck_campaigns(){
  _socialism_check_for_stuck_campaigns();
}

function drush_socialism_finish_campaigns(){
  _socialism_finish_campaigns();
}

/**
 * Implements hook_drush_cache_clear().
 */
function socialism_drush_cache_clear(&$types) {
  if (function_exists('module_exists') && module_exists('socialism')) {
    $types['adroll'] = 'drush_socialism_cache_clear';
  }
}

/**
 * Clear caches internal to Socialsm module.
 */
function drush_socialism_cache_clear() {
  cache_clear_all("adroll_campaigns", "cache_adroll");
  cache_clear_all("adroll_segments", "cache_adroll");
  cache_clear_all("adroll_ads", "cache_adroll");
  cache_clear_all("adroll_pixel", "cache_adroll");
}

