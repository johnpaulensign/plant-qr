class Plant {
  constructor(id, commonName, scientificName) {
    this.id = id;
    this.commonName = commonName;
    this.scientificName = scientificName;
    this.maintenanceTasks = {
      lastWatered: [],
      lastFertilized: [],
      lastPestTreatment: []
    };
  }

  waterPlant(date = new Date()) {
    this.maintenanceTasks.lastWatered.push(date);
  }

  fertilizePlant(date = new Date()) {
    this.maintenanceTasks.lastFertilized.push(date);
  }

  pestTreatment(date = new Date()) {
    this.maintenanceTasks.lastPestTreatment.push(date);
  }
}