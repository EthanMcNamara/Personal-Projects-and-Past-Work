const request = require("supertest");
const baseURL = "http://localhost:3000";

// Tests all GET functions
describe("DELETE /poll", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).delete("/poll");
        expect(response.statusCode).toBe(405);
    });
});

describe("DELETE /poll/:id", () => {
    const validPoll = "plw2024";
    const invalidPoll = "gibberish";
    it("should return 405 on existing pollId", async () => {
        const response = await request(baseURL).delete("/poll/"+validPoll);
        expect(response.statusCode).toBe(405);
    });
    it("should return 405 on invalid pollId", async () => {
        const response = await request(baseURL).delete("/poll/"+invalidPoll);
        expect(response.statusCode).toBe(405);
    });
});

describe("DELETE /vote", () => {
    it("should return 405", async () => {
        const response = await request(baseURL).delete("/vote");
        expect(response.statusCode).toBe(405);
    });
});

describe("DELETE /vote/:id", () => {
    const gibberish = "thisMakesNoSense";
    const validVote = {
        "pollId": "plw2024",
        "optionId": 1
    };
    var voteId = "";
    beforeAll(async () => {
        // Ensure there is at least 1 vote before checking for votes
        const response = await request(baseURL).post("/vote").send(validVote);
        voteId = response.body;
    });
    it("should return 200 when vote is successfully deleted", async () => {
        const response = await request(baseURL).delete("/vote/"+voteId);
        expect(response.status).toBe(200);
    });
    it("should return 400 if an attempt is made to delete a vote that no longer exists", async () => {
        const response = await request(baseURL).delete("/vote/"+voteId);
        expect(response.status).toBe(400);
    });
    it("should return 400 when parameter does not match an existing vote's id", async () => {
        const response = await request(baseURL).delete("/vote/"+gibberish);
        expect(response.status).toBe(400);
    });
});


describe("DELETE /*", () => {
    const gibberish = "thisMakesNoSense";
    const validPoll = "plw2024";
    it("should return 404 for empty space after baseURL", async () => {
        const response = await request(baseURL).delete("/");
        expect(response.status).toBe(404);
    });
    it("should return 404 if unexpected route given after baseURL", async () => {
        const response = await request(baseURL).delete("/"+gibberish);
        expect(response.status).toBe(404);
    });
    it("should return 404 if valid pollId given after baseURL without '/poll/'", async () => {
        const response = await request(baseURL).delete("/"+validPoll);
        expect(response.status).toBe(404);
    });
});