<?php
/**
 * @file
 * Migration script for users migration.
 */

/**
 *
 * Migrate Backend Users
 */
class UserMigration extends BaseMigration {
    public function __construct() {
    parent::__construct();
    $this->description = t('XML feed of wine producers of the world');
    //$this->dependencies = array('WineRegion', 'WineUser');

    // There isn't a consistent way to automatically identify appropriate "fields"
    // from an XML feed, so we pass an explicit list of source fields
    $fields = array(
      'create_time' => t('User create date'),
      'name' => t('User name'),
      'email' => t('User email'),
      'level' => t('Role of user'),
    );

    // The source ID here is the one retrieved from the XML listing file, and
    // used to identify the specific item's file
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sourceid' => array(
          'type' => 'varchar',
          'length' => 4,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // IMPORTANT: Do not try this at home! We have included importable files
    // with the migrate_example module so it can be very simply installed and
    // run, but you should never include any data you want to keep private
    // (especially user data like email addresses, phone numbers, etc.) in the
    // module directory. Your source data should be outside of the webroot, and
    // should not be anywhere where it may get committed into a revision control
    // system.

    // This can also be an URL instead of a file path.
    $folder = DRUPAL_ROOT . '/' . drupal_get_path('module', 'bigmedium_cms_migration') . '/data';
    $list_url = $folder . 'index.cgi';
    // Each ID retrieved from the list URL will be plugged into :id in the
    // item URL to fetch the specific objects.
    $item_url = $folder . ':id.cgi';
	$list_dir = array($folder);
	$base_dir = DRUPAL_ROOT . '/' . drupal_get_path('module', 'bigmedium_cms_migration');
    // We use the MigrateSourceList class for any source where we obtain the list
    // of IDs to process separately from the data for each item. The listing
    // and item are represented by separate classes, so for example we could
    // replace the XML listing with a file directory listing, or the XML item
    // with a JSON item.
    $this->source = new MigrateSourceList(new MigrateListFiles($list_dir , $base_dir , '/(.*\.cgi$)/i'),new MigrateItemFile($base_dir), $fields);

    $this->destination = new MigrateDestinationUser();

    // TIP: Note that for XML sources, in addition to the source field passed to
    // addFieldMapping (the name under which it will be saved in the data row
    // passed through the migration process) we specify the Xpath used to retrieve
    // the value from the XML.
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('created', 'create_time');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('roles', 'level');


  }
 
}


