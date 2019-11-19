<?php

/**
 * @file
 * Apply Parent Compound Objects policies onto Children.
 */

/**
 * Apply compound object policies to children if is ingested or modified.
 *
 * @param AbstractObject $object
 *   An AbstractObject of the parent object.
 * @param AbstractDatastream $datastream
 *   An AbstractDatastream of the datastream added to the parent object.
 */
function wwudora_inherit_xacml_from_compound_parent(AbstractObject $object, AbstractDatastream $datastream) {
  if ($datastream->id == "POLICY" && in_array("islandora:compoundCModel", $object->models)) {
    $children = islandora_compound_object_get_parts($object, TRUE);
    foreach ($children as $child) {
      $childObj = islandora_object_load($child['pid']);
      $childObj->relationships->remove(ISLANDORA_RELS_EXT_URI, 'inheritXacmlFrom', NULL, RELS_TYPE_URI);
      $childObj->relationships->add(ISLANDORA_RELS_EXT_URI, 'inheritXacmlFrom', $object->id, RELS_TYPE_URI);
      $xacml = new IslandoraXacml($childObj, $object['POLICY']->content);
      $xacml->writeBackToFedora();
    }
  } 
}
