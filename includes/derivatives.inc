<?php

/**
 * @file
 * Contains derivative functionality for WWU.
 */

/**
 * Update child polcies of a parent compound object object.
 *
 * @param AbstractObject $parent
 *   The parent compound object.
 * @param bool $force
 *   Boolean for forcing this derivative creation.
 *
 * @return array
 *   Success message.
 */
function wwudora_policy_to_children_derivatives(AbstractObject $parent, $force = FALSE) {
  try {
    $children = islandora_compound_object_get_parts($parent, TRUE);
    if ($children) {
      foreach ($children as $child) {
        $child_obj = islandora_object_load($child['pid']);
        wwudora_inherit_xacml_from_compound_parent($parent, $child_obj);
      }
    }
  }
  catch (Exception $e) {
    return array(
      'success' => FALSE,
      'messages' => array(
        array(
          'message' => t('Failed to update @dsid on Children of @parent.'),
          'message_sub' => array(
            '@dsid' => 'POLICY',
            '@parent' => $parent->id,
          ),
          'type' => 'watchdog',
          'severity' => WATCHDOG_ERROR,
        ),
      ),
    );
  }

  return array(
    'success' => TRUE,
    'messages' => array(
      array(
        'message' => t('Updated @dsid on Children of @parent.'),
        'message_sub' => array(
          '@dsid' => 'POLICY',
          '@parent' => $parent->id,
        ),
        'type' => 'dsm',
        'severity' => 'status',
      ),
    ),
  );
}

/**
 * Apply compound object policies to a child.
 *
 * @param AbstractObject $parent
 *   An AbstractObject of the parent object.
 * @param AbstractObject $child
 *   An AbstractObject of the child object.
 */
function wwudora_inherit_xacml_from_compound_parent(AbstractObject $parent, AbstractObject $child) {

  // Removes any existing inheritXacmlFrom relationships from other parents.
  if (!$child->relationships->get(ISLANDORA_RELS_EXT_URI, 'inheritXacmlFrom', $parent->id, RELS_TYPE_URI)) {
    $child->relationships->remove(ISLANDORA_RELS_EXT_URI, 'inheritXacmlFrom', NULL, RELS_TYPE_URI);
    $child->relationships->add(ISLANDORA_RELS_EXT_URI, 'inheritXacmlFrom', $parent->id, RELS_TYPE_URI);
  }

  // Settings Child's Policy to the Parent's.
  if (isset($parent['POLICY']) && (!isset($child['POLICY']) || !($parent['POLICY']->content === $child['POLICY']->content))) {
    $xacml = new IslandoraXacml($child, $parent['POLICY']->content);
    $xacml->writeBackToFedora();
  }

  // Removing Child's existing Policy if the Parent doesn't have one.
  elseif (isset($child['POLICY']) && !isset($parent['POLICY'])) {
    $xacml = new IslandoraXacml($child);
    $xacml->datastreamRule->clear();
    $xacml->managementRule->clear();
    $xacml->viewingRule->clear();
    $xacml->writeBackToFedora();
  }
}
