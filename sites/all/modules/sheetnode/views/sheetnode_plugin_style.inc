<?php

require_once(drupal_get_path('module', 'sheetnode') . '/socialcalc.inc');

define('SHEETNODE_EXPANSION_VERTICAL', 0);
define('SHEETNODE_EXPANSION_HORIZONTAL', 1);

class sheetnode_plugin_style extends views_plugin_style {
  function option_definition() {
    $options = parent::option_definition();
    $options['expansion'] = array('default' => SHEETNODE_EXPANSION_VERTICAL); 
    $options['use_template'] = array('default' => FALSE);
    $options['template'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_values) {
    parent::options_form($form, $form_values);
    $form['expansion'] = array(
      '#type' => 'radios',
      '#title' => t('Expansion of results'),
      '#description' => t('You can specify whether view results should be expanded horizontally or vertically.'),
      '#options' => array(
        SHEETNODE_EXPANSION_VERTICAL => t('Consecutive rows'),
        SHEETNODE_EXPANSION_HORIZONTAL => t('Consecutive columns'),
      ),
      '#default_value' => $this->options['expansion'],
      '#weight' => 97,
    );
    $form['use_template'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use template'),
      '#description' => t('Check this box to use the spreadsheet below as template for your view. 
                           To place view results in the template, use the following syntax in the cells:
                           <ul>
                           <li><code>${field_label}</code> to indicate that this cell and subsequent ones should be filled with values of this field.</li>
                           <li><code>${FUNCTION(@field_label@, $field_label$, @cell_reference@, ...)}</code> to indicate that this cell and subsequent ones should be filled with a calculation.
                           <code>@field_label@</code> are replaced with the corresponding cell references, and <code>$field_label$</code> are replaced with actual field values.</li>
                           <li><code>$[FUNCTION(@field_label@, @cell_reference@, ...)]</code> to indicate that a calculation should be placed in this cell. 
                           <code>@field_label@</code> are replaced with the corresponding cell ranges.</li>
                           </ul>'),
      '#default_value' => $this->options['use_template'],
      '#weight' => 98,
    );
    $id = 'edit-style-options-template';
    $form['template'] = array(
      '#type' => 'hidden', 
      '#attributes' => array('id' => $id), 
      '#weight' => 99
    );
    $this->render_inject($this->options['template'], $id, $form['template'], '#prefix');
  }

  function render_sheet() {
    $tangent = $this->options['expansion'];
    $normal  = 1-$tangent;
    if ($this->options['use_template']) {
      $socialcalc = socialcalc_parse($this->options['template']);
      $lastpos = array(0,0);

      // Iterate through cells, gathering placeholder values.
      if (!empty($socialcalc['sheet']['cells'])) foreach ($socialcalc['sheet']['cells'] as $coord => $cell) {
        // Field placeholder?
        $matches = array();
        if (isset($cell['datavalue']) && $cell['datatype'] == 't') {
          if (preg_match('/^\${(.*?)\}$/', $cell['datavalue'], $matches)) {
            $field_name = $this->find_field($matches[1]);
            if (isset($cell['comment']) && preg_match('/^\${(.*?)\}$/', $cell['comment'], $comment_matches)) {
              $comment_field_name = $this->find_field($comment_matches[1]);
            }
            else {
              $comment_field_name = FALSE;
            }
            if ($field_name && !$this->view->field[$field_name]->options['exclude']) { // it's a field expansion
              $fields[$field_name] = array(
                'coord' => $coord,
                'cell' => $cell,
                'info' => $this->view->field[$field_name],
                'comment' => $comment_field_name,
              );
            }
            else { // it's a formula expansion
              $formula_expansions[] = array(
                'coord' => $coord,
                'cell' => $cell,
                'expression' => $matches[1],
                'comment' => $comment_field_name,
              );
            }
          }
          else if (preg_match('/^\$\[(.*?)]$/', $cell['datavalue'], $matches)) { // it's a formula placement
            $formulas[] = array(
              'coord' => $coord,
              'cell' => $cell,
              'expression' => $matches[1],
            );
          }
        }
        $lastpos = array(max($lastpos[0], $cell['pos'][0]), max($lastpos[1], $cell['pos'][1]));
      }

      // Replace field placeholders with actual values.
      $tangent_increment = array(SHEETNODE_EXPANSION_VERTICAL => 0, SHEETNODE_EXPANSION_HORIZONTAL => 1);
      if (!empty($fields)) foreach ($fields as $field_name => $field) {
        unset($socialcalc['sheet']['cells'][$field['coord']]);
        $newcell = $field['cell'];
        $pos = $maxpos = $newcell['pos'];
        $this->view->row_index = 0;
        foreach ($this->view->result as $j => $result) {
          $this->view->row_index = $j;

          // Get comment value if any.
          if ($field['comment']) {
            $comment_field = $this->view->field[$field['comment']];
            $newcell['comment'] = strip_tags($comment_field->theme($result));
          }

          // Get cell value.
          $value = $field['info']->theme($result);
          if (is_array($value)) {
            // Expand an array of values into consecutive cells.
            $itempos = $pos;
            foreach ($value as $item) {
              $itemcell = $newcell;
              $itemcell['pos'] = $itempos;
              $itemcell['datavalue'] = $item;
              $itemcell['datatype'] = is_numeric($item) ? 'v' : 't';
              $itemcell['valuetype'] = is_numeric($item) ? 'n' : 'th';
              $newcells[socialcalc_cr_to_coord($itempos[0], $itempos[1])] = $itemcell;
              $itempos[$tangent]++;
              $maxpos[$tangent] = max($maxpos[$tangent], $itempos[$tangent]);
            }
          }
          else {
            // Expand a single value into the current cell.
            $newcell['pos'] = $pos;
            $newcell['datavalue'] = $value;
            $newcell['datatype'] = is_numeric($value) ? 'v' : 't';
            $newcell['valuetype'] = is_numeric($value) ? 'n' : 'th';
            $newcells[socialcalc_cr_to_coord($pos[0], $pos[1])] = $newcell;
          }
          $pos[$normal]++;
        }
        $maxpos[$normal] = $pos[$normal];
        $fields[$field_name]['endpos'] = array($maxpos[0]-$tangent_increment[$tangent], $maxpos[1]-$tangent_increment[$normal]);
        $lastpos = array(max($lastpos[0], $maxpos[0]), max($lastpos[1], $maxpos[1]));
      }

      // Replace formula expansions with actual values.
      if (!empty($formula_expansions) && !empty($fields)) foreach ($formula_expansions as $fe) {
        unset($socialcalc['sheet']['cells'][$fe['coord']]);
        $newcell = $fe['cell'];
        $pos = $newcell['pos'];
        $matches = array();
        $count = preg_match_all('/@(.*?)@|\$(.*?)\$/', $fe['expression'], $matches);
        $this->view->row_index = 0;
        foreach ($this->view->result as $j => $result) {
          $this->view->row_index = $j;

          // Get comment value if any.
          if ($fe['comment']) {
            $comment_field = $this->view->field[$fe['comment']];
            $newcell['comment'] = strip_tags($comment_field->theme($result));
          }

          // Expand formula.
          $expression = $fe['expression'];
          $newcell['pos'] = $pos;
          for ($i=0; $i<$count; $i++) {
            $replace = NULL;
            $match = !empty($matches[1][$i]) ? $matches[1][$i] : $matches[2][$i];
            $field_name = $this->find_field($match);
            if (!$field_name) {
              // Not a field: try a cell coordinate.
              if (preg_match('/^\$?\w\w?\$?\d+$/', $match)) {
                $refpos = socialcalc_coord_to_cr($match);
                foreach ($fields as $field) {
                  if ($refpos == $field['cell']['pos']) {
                    $replace = socialcalc_cr_to_coord(
                      $field['cell']['pos'][0]+($j*$tangent_increment[$tangent]),
                      $field['cell']['pos'][1]+($j*$tangent_increment[$normal])
                    );
                    break;
                  }
                }
              }
            } else if ($matches[0][$i][0] == '@' && isset($fields[$field_name])) {
              $replace = socialcalc_cr_to_coord(
                $fields[$field_name]['cell']['pos'][0]+($j*$tangent_increment[$tangent]),
                $fields[$field_name]['cell']['pos'][1]+($j*$tangent_increment[$normal])
              );
            }
            else if ($matches[0][$i][0] == '$') {
              $field = $this->view->field[$field_name];
              $replace = $field->get_value($result);
              // Special case: empty array == NULL.
              if (is_array($replace) && empty($replace)) {
                $replace = NULL;
              }
              // Otherwise, if array of values, render them.
              else if (!is_scalar($replace) && isset($field->field_info) && isset($result->_field_data[$field->field_alias])) {
                $field_data = $result->_field_data[$field->field_alias];
                $output = '';
                foreach ($replace as $item) {
                  $element = field_view_value($field_data['entity_type'], $field_data['entity'], $field_name, $item);
                  $output .= drupal_render($element);
                }
                $replace = $output;
              }
            }
            if (!is_null($replace)) {
              $expression = str_replace($matches[0][$i], $replace, $expression);
            }
            else {
              $expression = NULL;
            }
          }
          if (!is_null($expression)) {
            $newcell['formula'] = $expression;
            $newcell['datatype'] = 'f';
            $newcell['valuetype'] = 'n';
            $newcell['datavalue'] = 0;
            $newcells[socialcalc_cr_to_coord($pos[0], $pos[1])] = $newcell;
          }
          $pos[$normal]++;
        }
        $fields[] = array(
          'cell' => $fe['cell'],
          'coord' => $fe['coord'],
          'endpos' => array($pos[0]-$tangent_increment[$tangent], $pos[1]-$tangent_increment[$normal]),
        );
        $lastpos = array(max($lastpos[0], $pos[0]), max($lastpos[1], $pos[1]));
      }

      // Replace formula placeholders with actual values.
      if (!empty($formulas)) foreach ($formulas as $formula) {
        $newcell = $formula['cell'];
        $expression = $formula['expression'];
        $matches = array();
        $count = (int)preg_match_all('/@(.*?)@/', $expression, $matches);
        for ($i=0; $i<$count; $i++) {
          $field_name = $this->find_field($matches[1][$i]);
          if (!$field_name || !isset($fields[$field_name])) { 
            // Not a field: try a cell coordinate.
            if (preg_match('/^\$?\w\w?\$?\d+$/', $matches[1][$i])) {
              $replace = '@'.$matches[1][$i].'@';
              $references[$formula['coord']][] = socialcalc_coord_to_cr(str_replace('$', '', $matches[1][$i]));
            }
            else {
              $replace = NULL;
            }
          }
          else {
            $field = $fields[$field_name];
            $replace = socialcalc_cr_to_coord($field['cell']['pos'][0], $field['cell']['pos'][1]) .':'. socialcalc_cr_to_coord($field['endpos'][0], $field['endpos'][1]);
          }
          if (!is_null($replace)) {
            $expression = str_replace($matches[0][$i], $replace, $expression);
          }
          else {
            $expression = NULL;
          }
        }
        if (!is_null($expression)) {
          $newcell['formula'] = $expression;
          $newcell['datatype'] = 'f';
          $newcell['valuetype'] = 'n';
          $newcell['datavalue'] = 0;
          $socialcalc['sheet']['cells'][$formula['coord']] = $newcell;
        }
      }

      // Adjust positions of all cells based on expanded values.
      if (!empty($fields)) foreach ($socialcalc['sheet']['cells'] as $coord => $cell) {
        $pos = $cell['pos'];
        foreach ($fields as $field) {
          if ($pos[$tangent] == $field['cell']['pos'][$tangent] && $pos[$normal] > $field['cell']['pos'][$normal]) {
            $pos[$normal] += $field['endpos'][$normal] - $field['cell']['pos'][$normal];
            unset($socialcalc['sheet']['cells'][$coord]);
            $cell['pos'] = $pos;
            $newcoord = socialcalc_cr_to_coord($pos[0], $pos[1]);
            $socialcalc['sheet']['cells'][$newcoord] = $cell;
            if (isset($references[$coord])) {
              $references[$newcoord] = $references[$coord];
              unset($references[$coord]);
            }
            $lastpos = array(max($lastpos[0], $pos[0]), max($lastpos[1], $pos[1]));
            break;
          }
        }
      }

      // Adjust references in formulas based on expanded values.
      if (!empty($fields) && !empty($references)) foreach ($references as $coord => $positions) {
        $cell = $socialcalc['sheet']['cells'][$coord];
        $expression = $cell['formula'];
        $matches = array();
        $count = (int)preg_match_all('/@\$?\w\w?\$?\d+@/', $expression, $matches);
        for ($i=0; $i<$count; $i++) {
          $pos = $positions[$i];
          foreach ($fields as $field) {
            if ($pos[$tangent] == $field['cell']['pos'][$tangent]) {
              if ($pos[$normal] == $field['cell']['pos'][$normal]) { // referencing a field cell
                $refrange = socialcalc_cr_to_coord($field['cell']['pos'][0], $field['cell']['pos'][1]) .':'. socialcalc_cr_to_coord($field['endpos'][0], $field['endpos'][1]);
                $expression = str_replace($matches[0][$i], $refrange, $expression);
                break;
              } else if ($pos[$normal] > $field['cell']['pos'][$normal]) { // referencing a cell below a field
                $pos[$normal] += $field['endpos'][$normal] - $field['cell']['pos'][$normal];
                $refcoord = socialcalc_cr_to_coord($pos[0], $pos[1], TRUE);
                $expression = preg_replace('/@(\$?)\w\w?(\$?)\d+@/', '${1}'.$refcoord[0].'${2}'.$refcoord[1], $expression, 1);
                break;
              }
            }
          }
        }
        $cell['formula'] = $expression;
        $socialcalc['sheet']['cells'][$coord] = $cell;
      }

      // Replace values inside the sheet.
      if (!empty($newcells)) {
        $socialcalc['sheet']['cells'] += $newcells;
      }
      $socialcalc['sheet']['attribs']['lastcol'] = $lastpos[0];
      $socialcalc['sheet']['attribs']['lastrow'] = $lastpos[1];

      // Adjust starting cell in editor.
      $pos = $socialcalc['edit']['ecell']['pos'];
      if (!empty($fields)) foreach ($fields as $field) {
        if ($pos[$tangent] == $field['cell']['pos'][$tangent] && $pos[$normal] > $field['cell']['pos'][$normal]) {
          $pos[$normal] += $field['endpos'][$normal] - $field['cell']['pos'][$normal];
          $ecell_changed = TRUE;
          break;
        }
      }
      $socialcalc['edit']['ecell']['pos'] = $pos;
      $socialcalc['edit']['ecell']['coord'] = socialcalc_cr_to_coord($pos[0], $pos[1]);
      
      // Adjust row and column panes.
      $panes = $tangent == SHEETNODE_EXPANSION_VERTICAL ? 'rowpanes' : 'colpanes';
      foreach ($socialcalc['edit'][$panes] as $i => $pane) {
        $delta = 0;
        if (!empty($fields)) foreach ($fields as $field) {
          if ($pane['last'] > $field['cell']['pos'][$normal]) {
            $delta = max($delta, $field['endpos'][$normal] - $field['cell']['pos'][$normal]); 
          }
        }
        if (!empty($ecell_changed)) {
          $socialcalc['edit'][$panes][$i]['first'] += $delta;
          $socialcalc['edit'][$panes][$i]['last']  += $delta;
        }
      }
    }
    else {
      // Hand-make default SocialCalc structure based on views results.
      $pos = array(1,1);
      foreach ($this->view->field as $field => $info) {
        if ($info->options['exclude']) continue;
        $cell['pos'] = $pos;
        $cell['datavalue'] = $info->label();
        $cell['datatype'] = 't';
        $cell['valuetype'] = 't';

        $sheet['cells'][socialcalc_cr_to_coord($pos[0], $pos[1])] = $cell;
        $pos[$tangent]++;
      }
      $pos[$normal] = 2;
      $this->view->row_index = 0;
      foreach ($this->view->result as $j => $result) {
        $this->view->row_index = $j;
        $pos[$tangent] = 1;
        foreach ($this->view->field as $field => $info) {
          if ($info->options['exclude']) continue;
          $value = $info->theme($result);
          if (is_array($value)) {
            foreach ($value as $item) {
              $cell['pos'] = $pos;
              $cell['datavalue'] = $item;
              $cell['datatype'] = is_numeric($item) ? 'v' : 't';
              $cell['valuetype'] = is_numeric($item) ? 'n' : 'th';
              $sheet['cells'][socialcalc_cr_to_coord($pos[0], $pos[1])] = $cell;
              $pos[$tangent]++;
            }
          }
          else {
            $cell['pos'] = $pos;
            $cell['datavalue'] = $value;
            $cell['datatype'] = is_numeric($value) ? 'v' : 't';
            $cell['valuetype'] = is_numeric($value) ? 'n' : 'th';
            $sheet['cells'][socialcalc_cr_to_coord($pos[0], $pos[1])] = $cell;
            $pos[$tangent]++;
          }
        }
        $pos[$normal]++;
      }
      $sheet['attribs']['lastcol'] = $pos[0]-1;
      $sheet['attribs']['lastrow'] = $pos[1]-1;

      if ($tangent == SHEETNODE_EXPANSION_VERTICAL) { 
        $edit['rowpanes'] = array(
          0 => array('first' => 1, 'last' => 1),
          1 => array('first' => 2, 'last' => $sheet['attribs']['lastrow']),
        );
        $edit['colpanes'] = array(
          0 => array('first' => 1, 'last' => $sheet['attribs']['lastcol']),
        );
      }
      else {
        $edit['colpanes'] = array(
          0 => array('first' => 1, 'last' => 1),
          1 => array('first' => 2, 'last' => $sheet['attribs']['lastcol']),
        );
        $edit['rowpanes'] = array(
          0 => array('first' => 1, 'last' => $sheet['attribs']['lastrow']),
        );
      }
      $edit['ecell'] = array(
        'coord' => 'A1',
      );

      // Inject the Sheetnode code.
      $socialcalc = array(
        'sheet' => $sheet,
        'edit' => $edit,
        'audit' => socialcalc_default_audit($sheet),
      );
    }
    
    // Merge with saved version if any.
    $this->merge_annotation($socialcalc);

    unset($this->view->row_index);
    return $socialcalc;
  }

  function merge_annotation(&$original) {
    $annotation = db_query("SELECT value FROM {sheetnode_view} WHERE view_name = :name AND display_id = :id", array(
      ':name' => $this->view->name,
      ':id' => $this->view->display_handler->display->id,
    ))->fetchField();
    if ($annotation) {
      $merge = socialcalc_parse($annotation);

      // TODO - This is the wrong way to merge, because annotations will wind up
      // on the wrong rows if the view contains new records or is sorted differently.
      // What's needed is to associate each row with a record id to match them up when merging.
      foreach ($merge['sheet']['cells'] as $coord => $cell) {
        if (empty($original['sheet']['cells'][$coord])) {
          $original['sheet']['cells'][$coord] = $cell;

          // TODO - Import annotation styles into original.
        }
      }

      // Adjust lastcol and lastrow.
      $original['sheet']['attribs']['lastcol'] = max($original['sheet']['attribs']['lastcol'], $merge['sheet']['attribs']['lastcol']);
      $original['sheet']['attribs']['lastrow'] = max($original['sheet']['attribs']['lastrow'], $merge['sheet']['attribs']['lastrow']);
    }
  }

  function render_inject($value, $save_element, &$element, $attribute) {
    $path = drupal_get_path('module', 'sheetnode');
    $context = array('entity-type' => 'view', 'oid' => $this->view->name);
    $sheet_id = drupal_clean_css_identifier('sheetview-' . $this->view->name . (empty($this->view->live_preview) ? '' : '-preview'));
    $setting = array('sheetnode' => array($sheet_id => array(
      'aliases' => array($this->view->name),
      'value' => $value,
      'imagePrefix' => base_path() . $path . '/socialcalc/images/sc-',
      'containerElement' => $sheet_id,
      'saveElement' => $save_element,
      'viewMode' => variable_get('sheetnode_view_mode', SHEETNODE_VIEW_FIDDLE),
      'showToolbar' => variable_get('sheetnode_view_toolbar', FALSE),
      'permissions' => array(
        'edit sheetnode settings' => user_access('edit sheetnode settings'),
      ),
      'context' => $context,
    )));
    
    $element[$attribute] = '<div class="sheetview" id="' . $sheet_id . '"></div>';
    $element['#attached']['js'] = array(
      $path . '/socialcalc/socialcalcconstants.js' => array('weight' => 1),
      $path . '/socialcalc/socialcalc-3.js' => array('weight' => 2),
      $path . '/socialcalc/socialcalctableeditor.js' => array('weight' => 3),
      $path . '/socialcalc/formatnumber2.js' => array(),
      $path . '/socialcalc/formula1.js' => array(),
      $path . '/socialcalc/socialcalcpopup.js' => array('weight' => 4),
      $path . '/socialcalc/socialcalcspreadsheetcontrol.js' => array('weight' => 5),
      $path . '/socialcalc/socialcalcviewer.js' => array('weight' => 5),
      $path . '/sheetnode.js' => array('weight' => 6),
      array('data' => $setting, 'type' => 'setting'),
    );
    $element['#attached']['css'] = array(
      $path . '/socialcalc/socialcalc.css',
      $path . '/sheetnode.css',
    );

    module_invoke_all('sheetnode_plugins', $value, $save_element, $context);
  }

  function render() {
    foreach ($this->view->field as $field => $info) {
      // Don't render here if we're making an editable spreadsheet view.
      if ($info instanceof sheetnode_handler_field_editable) return;
    }
    $value = socialcalc_save($this->render_sheet());
    $element = array();
    $this->render_inject($value, FALSE, $element, '#markup');
    return $element;
  }

  function find_field($field_name) {
    if (isset($this->view->field[$field_name])) return $field_name;
    foreach ($this->view->field as $field => $info) {
      if ($info->field_alias == $field_name) return $field;
      if (strcasecmp($info->label(), $field_name) == 0) return $field;
      if (method_exists($info, 'ui_name') && strcasecmp($info->ui_name(), $field_name) == 0) return $field;
    }
    return FALSE;
  }

  function attach_to($display_id, $path, $title) {
    $url_options = array('html' => TRUE);
    $input = $this->view->get_exposed_input();
    if ($input) {
      $url_options['query'] = $input;
    }
    $image = theme('image', array('path' => $this->feed_image));
    $this->view->feed_icon .= l($image, $this->view->get_url(NULL, $path), $url_options);
  }
}

class sheetnode_feed_plugin_style extends sheetnode_plugin_style {
  function option_definition() {
    $options = parent::option_definition();
    $options['inherit'] = array('default' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_values) {
    parent::options_form($form, $form_values);
    $form['inherit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Inherit spreadsheet settings'),
      '#default_value' => $this->options['inherit'],
      '#description' => t('If this feed is attached to a display that has a Spreadsheet style, 
                           this option lets you inherit the settings of that style, including the spreadsheet formatting. 
                           The settings below are then ignored.'),
    );
  }

  function inherit_options() {
    if ($this->options['inherit']) {
      $displays = array_filter($this->display->handler->get_option('displays'));
      if (!empty($displays)) foreach ($displays as $display) {
        $handler = $this->view->display[$display]->handler;
        if ($handler->get_option('style_plugin') == 'sheet') {
          $options = $handler->get_option('style_options');
          break;
        }
      }
      if (isset($options)) foreach ($options as $key => $option) {
        $this->options[$key] = $option;
      }
    }
  }
}

